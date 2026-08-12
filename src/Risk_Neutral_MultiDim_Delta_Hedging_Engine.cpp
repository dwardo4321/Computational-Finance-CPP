#include "Risk_Neutral_MultiDim_Delta_Hedging_Engine.hpp"
#include "utilities.hpp"
#include "Asset_Option_Price.hpp"

#include <Eigen/Dense>
#include <Eigen/Core>
#include <boost/math/distributions.hpp>
#include <iostream>
#include <cmath>
#include <random>
#include <optional>
#include <functional>
#include <vector>

namespace{
    std::random_device rd;
    std::mt19937_64 gen(rd());
}

using namespace Eigen::placeholders;

// Constructor ----------------------------------------------------------------------------
Multidimensional_Risk_Neutral_Engine::Multidimensional_Risk_Neutral_Engine(const double& strike_const, 
                                                                            const Eigen::VectorXd& rate_const, 
                                                                            const double& risk_free_rate_const, 
                                                                            const Eigen::VectorXd& price_today_const,
                                                                                                                                                                                    
                                                                            const Eigen::MatrixXd& volatility_realised_const, 
                                                                            const Eigen::MatrixXd& volatility_implied_const, 
                                                                                                                                                                                    
                                                                            const double& Time_const,
                                                                            const int& discretisation_const):

                                                    strike(strike_const),  
                                                    rate(rate_const),         // fixed rates
                                                    risk_free_rate(risk_free_rate_const),
                                                    price_today(price_today_const),

                                                    volatility_realised(volatility_realised_const),   // assets volatility
                                                    volatility_implied(volatility_implied_const),  // option volatilities
                                                    
                                                    Time(Time_const),                  // time duration
                                                    discretisation(discretisation_const)
                                                    {} // number of steps 


// Private Method 1 ---------------------------------------------------------
std::pair <Eigen::MatrixXd, Eigen::MatrixXd> Multidimensional_Risk_Neutral_Engine::Multidimensional_GBM(bool exact_gbm, double tau, int discretisation, const Eigen::MatrixXd& standard_normal_rv, std::optional<Eigen::MatrixXd> correlation_matrix, Eigen::VectorXd initial_price){

    int M = volatility_realised.rows();
    int D = volatility_realised.cols();
    int dimensions = volatility_realised.cols();

    Eigen::MatrixXd brownian_mot = utility::Brownian_path_generator(discretisation, dimensions, tau, standard_normal_rv, correlation_matrix);

    Eigen::VectorXd risk_premium = rate.array() - risk_free_rate;

    Eigen::VectorXd mqt_price_of_risk = volatility_realised.completeOrthogonalDecomposition().solve(risk_premium);
    
    brownian_mot.rowwise() += mqt_price_of_risk.transpose();
    
    Eigen::MatrixXd price(discretisation, initial_price.size());
    Eigen::MatrixXd price_variance_reduction(discretisation, initial_price.size());

    price.row(0) = initial_price.transpose();
    price_variance_reduction.row(0) = initial_price.transpose();

    Eigen::MatrixXd change_in_price = Eigen::MatrixXd::Zero(discretisation, initial_price.size());
    Eigen::MatrixXd change_in_price_vd = Eigen::MatrixXd::Zero(discretisation, initial_price.size());

    Eigen::MatrixXd vol = Eigen::MatrixXd::Zero(discretisation, M);
    Eigen::MatrixXd vol_vd = Eigen::MatrixXd::Zero(discretisation, M);

    double dt = tau / static_cast<double>(discretisation - 1);
    Eigen::VectorXd risk_free_rate_vector = Eigen::VectorXd::Constant(volatility_realised.rows(), risk_free_rate);

        if(exact_gbm){
            // Exact Multidimensional GBM
            for (int t = 1; t < discretisation; t++){
            
            auto step_covariance = volatility_realised * correlation_matrix.value_or(Eigen::MatrixXd::Identity(volatility_realised.cols(), volatility_realised.cols())) * volatility_realised.transpose();
            price.row(t) = initial_price.array() * ((risk_free_rate_vector - (0.5 * step_covariance).diagonal()) * (t * dt) + volatility_realised * brownian_mot.row(t).transpose()).array().exp();
            price_variance_reduction.row(t) = initial_price.array() * ((risk_free_rate_vector - (0.5 * step_covariance).diagonal()) * (t * dt) - volatility_realised * brownian_mot.row(t).transpose()).array().exp();
            }

        }else{
            // Euler-Maruyama Multidimensional GBM
            for (int t = 1; t < discretisation; t++){
        
                for(int j = 0; j < M; j++){

                    for (int i = 0; i < D; i++){
                        
                        vol(t, j) += volatility_realised(j, i) * (brownian_mot(t, i) - brownian_mot(t-1, i));
                        vol_vd(t, j) += volatility_realised(j, i) * -(brownian_mot(t, i) - brownian_mot(t-1, i));
                    } 
                }   

                change_in_price.row(t) = (risk_free_rate * price.row(t-1).array() * dt) + (price.row(t-1).array() * vol.row(t).array());
                change_in_price_vd.row(t) = (risk_free_rate * price_variance_reduction.row(t-1).array() * dt) + (price_variance_reduction.row(t-1).array() * vol_vd.row(t).array());
                
                price.row(t) = price.row(t-1) + change_in_price.row(t);
                price_variance_reduction.row(t) = price_variance_reduction.row(t-1) + change_in_price_vd.row(t);

            }
        }
    return {price, price_variance_reduction};
}

// Private Method 2 (Adjoint Algorithmic Differentiation and Likelihood Ratio Estimation) ---------------------------------------------------------
Multidimensional_Risk_Neutral_Engine::quad Multidimensional_Risk_Neutral_Engine::Greeks_and_Option(bool exact_gbm, int MC_iterations, double tau, bool variance_reduction,
                                                                                                    Eigen::VectorXd initial_price, std::optional<Eigen::MatrixXd> correlation_matrix,
                                                                                                    const std::vector<Eigen::MatrixXd>& standard_normal_rv_bank,
                                                                                                    const Payoff& payoff_object,
                                                                                                    std::function < std::pair<Eigen::MatrixXd, Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > custom_price_generator){

    
    // OPTION -------------------------------------------------------
    if(standard_normal_rv_bank.size() < MC_iterations){
        throw std::invalid_argument("The Standard Normal Bank contains fewer matrices than the number of iterations MC_iterations");
    }

    Asset_Option_Price Option_value = Asset_Option_Price(strike, rate, risk_free_rate, volatility_realised, initial_price, tau, discretisation);

    int path_index = 0;

    auto custom_func = [this, tau, &exact_gbm, &standard_normal_rv_bank, &initial_price, &path_index] (std::optional<Eigen::MatrixXd> correlation_matrix){

        const Eigen::MatrixXd& standard_normal_rv = standard_normal_rv_bank.at(path_index);

        path_index++;

        return this-> Multidimensional_GBM(exact_gbm , tau, this->discretisation, standard_normal_rv, correlation_matrix, initial_price);
    };
    
    double Option = Option_value.Monte_Carlo_option_pricer(MC_iterations, risk_free_rate, tau, variance_reduction, correlation_matrix, payoff_object, custom_func).sample_mean; 

    // GREEKS -------------------------------------------------------
    Eigen::VectorXd iter_price = Eigen::VectorXd::Zero(price_today.size());
    
    Eigen::VectorXd iter_delta;
    Eigen::VectorXd Delta = Eigen::VectorXd::Zero(price_today.size());

    double iter_theta;
    double Theta = 0.0;

    Eigen::MatrixXd iter_gamma(volatility_implied.rows(), volatility_implied.rows());
    Eigen::MatrixXd Gamma = Eigen::MatrixXd::Zero(volatility_implied.rows(), volatility_implied.rows());
    //Eigen::MatrixXd Gamma;// = Eigen::MatrixXd::Zero((discretisation, volatility_realised.cols()));

    //Eigen::VectorXd iter_norm_rv(volatility_implied.cols());

    double dt = tau / (discretisation - 1);

    Eigen::VectorXd risk_free_rate_vector = Eigen::VectorXd::Constant(volatility_realised.rows(), risk_free_rate);

    for(int i = 0; i < MC_iterations; i++){

        auto iter_gbm = Multidimensional_GBM(exact_gbm, tau, discretisation, standard_normal_rv_bank[i], correlation_matrix, initial_price);

        variance_reduction? (iter_price = iter_gbm.second(last, all).transpose()): (iter_price = iter_gbm.first(last, all).transpose());

        auto iter_norm_rv = ((sqrt(dt) * standard_normal_rv_bank[i].colwise().sum()).array() / sqrt(tau)).matrix();

        Eigen::MatrixXd asset_covariance = (volatility_realised * correlation_matrix.value_or(Eigen::MatrixXd::Identity(volatility_realised.cols(), volatility_realised.cols()))) * volatility_realised.transpose();

   
        // DELTA (Adjoint Algorithmic Differentiation) ---------------------------------------------------------

        iter_delta = (1.0 / MC_iterations) * exp(- risk_free_rate * tau) * (payoff_object.gradient(iter_price).cwiseProduct(iter_price.cwiseQuotient(initial_price)));

        Delta += iter_delta;


        // THETA (Adjoint Algorithmic Differentiation) ---------------------------------------------------------

        auto theta_valuation = risk_free_rate * payoff_object(iter_price); 

        auto theta_gradient = payoff_object.gradient(iter_price).transpose();

            auto theta_drift = risk_free_rate_vector - 0.5 * asset_covariance.diagonal(); 

            auto theta_corr_brownian_shock = (iter_price.cwiseQuotient(initial_price)).array().log() - theta_drift.array() * tau;

        auto theta_log_price_time_grad = theta_drift + (theta_corr_brownian_shock / (2 * tau)).matrix();

        auto theta_price_time_sensitivity = iter_price.cwiseProduct(theta_log_price_time_grad);

        iter_theta = (1.0 / MC_iterations) * exp(- risk_free_rate * tau) * (theta_valuation - theta_gradient * theta_price_time_sensitivity);

        Theta += iter_theta;


        // GAMMA (Finite Elements with Common Random Numbers)---------------------------------------------------------  

        auto e = Eigen::MatrixXd::Identity(initial_price.size(), initial_price.size());
        auto h = Eigen::VectorXd::Constant(0.5, initial_price.size(), initial_price.size());
        
        for(int i = 0; i < initial_price.size(); i++){

            for(int j = i + 1; j < initial_price.size(); j++){

                h(i) * e.col(j)

            }
        }

        /* auto iter_asset_covariance = asset_covariance * tau;
        
        auto iter_x = (log(iter_price.array() / initial_price.array()) - ((risk_free_rate_vector.array() - 0.5 * asset_covariance.diagonal().array()) * tau)).matrix();

        auto iter_A = (asset_covariance * tau).inverse();  // 5x5

        auto iter_q = iter_A * iter_x;  // 5x[5  5]x1 */

            // diagonal
            

            // off-diagonal
            /* for(int j = 0; j < 5; j++){
                for(int k = 0; k < 5; k++){
                    if(j != k){iter_gamma(j, k) = (1 / MC_iterations) * exp(-risk_free_rate * tau) * payoff_object(iter_price) * ((iter_q(j) * iter_q(k) - iter_A(j, k)) / (initial_price(j) * initial_price(k)));
                    }else{
                        auto gamma_weight = ((iter_q.array() * iter_q.array()).matrix() - iter_q - iter_A.diagonal()).array() / (initial_price.array() * initial_price.array());  // 5x1
                        auto gamma_diag = (1 / MC_iterations) * exp(-risk_free_rate * tau) * payoff_object(iter_price) * gamma_weight.matrix();  // 5x1
                        iter_gamma.diagonal() = gamma_diag;
                    }
                }
            }

            Gamma += iter_gamma; */
    }
    return {Delta, Gamma, Theta, Option};
}


/* Eigen::VectorXd Portfolio(int discretisation, double tau, Eigen::VectorXd initial_price){

    Eigen::VectorXd portfolio_value;

    auto[price, price_vd] = Multidimensional_GBM(tau, discretisation, correlation_matrix, initial_price);

    double portfolio_value = delta * price + bank;

} */


// Public Method 1 ---------------------------------------------------------
Eigen::MatrixXd Multidimensional_Risk_Neutral_Engine::Risk_Neutral_MultiDim_DHE(bool call){

    int time;
    
    double dt = Time / (discretisation - 1);

    double tau = Time - time * dt;
    
    /* Eigen::VectorXd out = Z_scores(true);//Brownian_Mot(discretisation, Time, std::nullopt); 

    Eigen::MatrixXd xxx(volatility_implied.rows(), volatility_implied.cols());
    xxx.col(0) = out;
    xxx.col(1) = out;
    auto[A, B] = Multidimensional_GBM(std::nullopt); */

    /* double dt = Time / (discretisation - 1);

    double tau = Time - 999 * dt;

    Asset_Option_Price Option_value = Asset_Option_Price(strike, rate, risk_free_rate, volatility_realised, price_today, tau, discretisation);

    auto custom_func = [this, tau] (std::optional<Eigen::MatrixXd> correlation_matrix){return this-> Multidimensional_GBM(tau, this->discretisation, correlation_matrix, this->price_today);};

    Eigen::VectorXd weights(5);
    weights << 0.3, 0.3, 0.2, 0.1, 0.05;

    Basket_Assets payoff_object(strike, weights);
    
    double out = Option_value.Monte_Carlo_option_pricer(1000, risk_free_rate, tau, true, std::nullopt, payoff_object, custom_func).sample_mean;  */

    Eigen::MatrixXd xxx(1, 2);
    xxx(0, 0) = 10;
    xxx(0, 1) = 20;

    return xxx;
}

