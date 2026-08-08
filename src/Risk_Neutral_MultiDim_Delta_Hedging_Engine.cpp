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
std::pair <Eigen::MatrixXd, Eigen::MatrixXd> Multidimensional_Risk_Neutral_Engine::Multidimensional_GBM(double tau, int discretisation, const Eigen::MatrixXd& standard_normal_rv, std::optional<Eigen::MatrixXd> correlation_matrix, Eigen::VectorXd initial_price){

    int M = volatility_realised.rows();
    int D = volatility_realised.cols();
    int dimensions = volatility_realised.cols();

    Eigen::MatrixXd brownian_mot = utility::Brownian_path_generator(discretisation, dimensions, tau, standard_normal_rv, correlation_matrix);

    Eigen::VectorXd risk_premium = rate.array() - risk_free_rate;

    Eigen::VectorXd mqt_price_of_risk = volatility_realised.completeOrthogonalDecomposition().solve(risk_premium);
    
    brownian_mot.rowwise() += mqt_price_of_risk.transpose();

    /*
    for(int i = 0; i < D; i++){
        for(int t = 0; t < discretisation; t++){
            brownian_mot(t, i) += mqt_price_of_risk(i);
        }
    }
    */
    
    Eigen::MatrixXd price(discretisation, initial_price.size());
    Eigen::MatrixXd price_variance_reduction(discretisation, initial_price.size());

    price.row(0) = initial_price.transpose();
    price_variance_reduction.row(0) = initial_price.transpose();

    Eigen::MatrixXd change_in_price = Eigen::MatrixXd::Zero(discretisation, initial_price.size());
    Eigen::MatrixXd change_in_price_vd = Eigen::MatrixXd::Zero(discretisation, initial_price.size());

    Eigen::MatrixXd vol = Eigen::MatrixXd::Zero(discretisation, M);
    Eigen::MatrixXd vol_vd = Eigen::MatrixXd::Zero(discretisation, M);

    double dt = tau / static_cast<double>(discretisation - 1);

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
   
    return {price, price_variance_reduction};
}

// // Private Method 2 (Adjoint Algorithmic Differentiation and Likelihood Ratio Estimation) ---------------------------------------------------------
Multidimensional_Risk_Neutral_Engine::quad Multidimensional_Risk_Neutral_Engine::Greeks_and_Option(int MC_iterations, double tau, bool variance_reduction,
                                                                                                    Eigen::VectorXd initial_price, std::optional<Eigen::MatrixXd> correlation_matrix,
                                                                                                    const std::vector<Eigen::MatrixXd>& standard_normal_rv_bank,
                                                                                                    const Payoff& payoff_object,
                                                                                                    std::function < std::pair<Eigen::MatrixXd, Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > custom_price_generator){

    

    if(standard_normal_rv_bank.size() < MC_iterations){
        throw std::invalid_argument("The Standard Normal Bank contains fewer matrices than the number of iterations MC_iterations");
    }

    Asset_Option_Price Option_value = Asset_Option_Price(strike, rate, risk_free_rate, volatility_realised, initial_price, tau, discretisation);

    int path_index = 0;

    auto custom_func = [this, tau, &standard_normal_rv_bank, &initial_price, &path_index] (std::optional<Eigen::MatrixXd> correlation_matrix){

        const Eigen::MatrixXd& standard_normal_rv = standard_normal_rv_bank.at(path_index);

        path_index++;

        return this-> Multidimensional_GBM(tau, this->discretisation, standard_normal_rv, correlation_matrix, initial_price);
    };
    
    double Option = Option_value.Monte_Carlo_option_pricer(MC_iterations, risk_free_rate, tau, variance_reduction, correlation_matrix, payoff_object, custom_func).sample_mean; 

    // GREEKS -------------------------------------------------------
    Eigen::VectorXd iter_price;
    
    //Eigen::VectorXd iter_delta;
    Eigen::VectorXd Delta = Eigen::VectorXd::Zero(price_today.size());

    Eigen::VectorXd theta_calc;
    Eigen::VectorXd iter_theta;
    Eigen::VectorXd Theta = Eigen::VectorXd::Zero(price_today.size());

    Eigen::VectorXd gamma_calc_num;
    Eigen::VectorXd gamma_calc_den;
    Eigen::MatrixXd iter_gamma;
    Eigen::MatrixXd Gamma = Eigen::MatrixXd::Zero(volatility_implied.rows(), volatility_implied.rows());

    //Eigen::VectorXd iter_norm_rv(volatility_implied.cols());

    double dt = tau / (MC_iterations - 1);

    for(int i = 0; i < MC_iterations; i++){

        auto iter_gbm = Multidimensional_GBM(tau, discretisation, standard_normal_rv_bank[i], correlation_matrix, initial_price);

        variance_reduction? (iter_price = iter_gbm.second(last, all)): (iter_price = iter_gbm.first(last, all));

        auto iter_norm_rv = ((sqrt(dt) * standard_normal_rv_bank[i].colwise().sum()).array() / sqrt(tau)).matrix();
   
        // DELTA ---------------------------------------------------------
        auto iter_delta = (1.0 / MC_iterations) * exp(- risk_free_rate * tau) * (iter_price.array() /  price_today.array()).matrix();

        Delta += iter_delta;

        /* // THETA ---------------------------------------------------------
        theta_calc = risk_free_rate * payoff_object(iter_price.transpose()) - iter_price.array() * (risk_free_rate - (0.5 * volatility_implied * volatility_implied).array() + (volatility_implied * iter_norm_rv).array() / (2 * sqrt(tau))).array();

        iter_theta = (1 / MC_iterations) * exp(- risk_free_rate * tau) * theta_calc;

        Theta += iter_theta; */

        // GAMMA ---------------------------------------------------------
        gamma_calc_num = (iter_norm_rv * iter_norm_rv).array() - 1 - iter_norm_rv.array() * volatility_implied.array() * sqrt(tau); 
        //gamma_calc_den = (price_today * price_today) * (volatility_implied * volatility_implied) * tau;

        //iter_gamma = (1 / MC_iterations) * exp(- risk_free_rate * tau) * payoff_object(iter_price.transpose()) * (gamma_calc_num.array() / gamma_calc_den.array());

        //Gamma += iter_gamma;

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

