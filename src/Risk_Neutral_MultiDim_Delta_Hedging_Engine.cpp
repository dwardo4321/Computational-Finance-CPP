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
    std::mt19937_64 generator(rd());
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
                                                    discretisation(discretisation_const),
                                                    M(volatility_realised_const.rows()),
                                                    D(volatility_realised_const.cols())
                                                    {} // number of steps 


// Public Method 1 ---------------------------------------------------------
std::pair <Eigen::MatrixXd, Eigen::MatrixXd> Multidimensional_Risk_Neutral_Engine::Multidimensional_GBM(bool exact_gbm, double tau, int discretisation, const Eigen::MatrixXd& standard_normal_rv, std::optional<Eigen::MatrixXd> correlation_matrix, Eigen::VectorXd initial_price){

    Eigen::MatrixXd brownian_mot = utility::Brownian_path_generator(discretisation, D, tau, standard_normal_rv, correlation_matrix);

    Eigen::VectorXd risk_premium = rate.array() - risk_free_rate;

    Eigen::VectorXd mqt_price_of_risk = volatility_realised.completeOrthogonalDecomposition().solve(risk_premium);
    
    brownian_mot.rowwise() += mqt_price_of_risk.transpose();
    
    Eigen::MatrixXd price = Eigen::MatrixXd::Zero(discretisation, M);
    Eigen::MatrixXd price_variance_reduction = Eigen::MatrixXd::Zero(discretisation, M);

    price.row(0) = initial_price.transpose();
    price_variance_reduction.row(0) = initial_price.transpose();

    Eigen::MatrixXd change_in_price = Eigen::MatrixXd::Zero(discretisation, M);
    Eigen::MatrixXd change_in_price_vd = Eigen::MatrixXd::Zero(discretisation, M);

    Eigen::MatrixXd vol = Eigen::MatrixXd::Zero(discretisation, M);
    Eigen::MatrixXd vol_vd = Eigen::MatrixXd::Zero(discretisation, M);

    double dt = tau / static_cast<double>(discretisation - 1);
    Eigen::VectorXd risk_free_rate_vector = Eigen::VectorXd::Constant(M, risk_free_rate); 

        if(exact_gbm){
            // Exact Multidimensional GBM
            for (int t = 1; t < discretisation; t++){
            
            auto step_covariance = volatility_realised * correlation_matrix.value_or(Eigen::MatrixXd::Identity(D, D)) * volatility_realised.transpose();
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

// Public Method 2 (Adjoint Algorithmic Differentiation and Finite Difference Method) ---------------------------------------------------------
Multidimensional_Risk_Neutral_Engine::quad Multidimensional_Risk_Neutral_Engine::Greeks_and_Option(bool exact_gbm, int MC_iterations, double tau, bool variance_reduction,
                                                                                                    Eigen::VectorXd initial_price, std::optional<Eigen::MatrixXd> correlation_matrix,
                                                                                                    const Payoff& payoff_object,
                                                                                                    std::function < std::pair<Eigen::MatrixXd, Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > custom_price_generator){

    // OPTION & GREEKS -------------------------------------------------------
    std::pair <Eigen::MatrixXd, Eigen::MatrixXd> iter_gbm;
    
    Eigen::VectorXd iter_price = Eigen::VectorXd::Zero(M);
    Eigen::VectorXd iter_price_vr = Eigen::VectorXd::Zero(M);

    double iter_option = 0;
    double iter_option_vr = 0;
    double Option = 0;
    
    Eigen::VectorXd iter_delta;
    Eigen::VectorXd iter_delta_vr;
    Eigen::VectorXd Delta = Eigen::VectorXd::Zero(M);

    double iter_theta;
    double iter_theta_vr;
    double Theta = 0.0;

    Eigen::MatrixXd iter_gamma = Eigen::MatrixXd::Zero(M, M);
    Eigen::MatrixXd iter_gamma_vr = Eigen::MatrixXd::Zero(M, M);
    Eigen::MatrixXd Gamma = Eigen::MatrixXd::Zero(M, M);

    auto risk_free_rate_vector = Eigen::VectorXd::Constant(M, risk_free_rate);

    auto asset_covariance = (volatility_realised * correlation_matrix.value_or(Eigen::MatrixXd::Identity(D, D))) * volatility_realised.transpose();

    // for Gamma calculation
    auto e = Eigen::MatrixXd::Identity(M, M);
    auto h = Eigen::VectorXd::Constant(M, 0.5);

    auto func_specific_discretisation = 2;

    for(int i = 0; i < MC_iterations; i++){

        auto standard_normal_rv = utility::Normal_RV_generator(func_specific_discretisation - 1, D, generator);       
        
        if(exact_gbm){iter_gbm = Multidimensional_GBM(true, tau, func_specific_discretisation, standard_normal_rv, correlation_matrix, initial_price);
        }else{iter_gbm = Multidimensional_GBM(false, tau, discretisation, standard_normal_rv, correlation_matrix, initial_price);}

        iter_price = iter_gbm.first(last, all).transpose();
        if(variance_reduction){iter_price_vr = iter_gbm.second(last, all).transpose();}

        
        // OPTION ---------------------------------------------------------

        iter_option = (1.0 / MC_iterations) * exp(- risk_free_rate * tau) * payoff_object(iter_price);

        if(variance_reduction){
            iter_option_vr = (1.0 / MC_iterations) * exp(- risk_free_rate * tau) * payoff_object(iter_price_vr);

            Option += 0.5 * (iter_option + iter_option_vr);

        }else{Option += iter_option;}

   
        // DELTA (Adjoint Algorithmic Differentiation) ---------------------------------------------------------

        iter_delta = (1.0 / MC_iterations) * exp(- risk_free_rate * tau) * (payoff_object.gradient(iter_price).cwiseProduct(iter_price.cwiseQuotient(initial_price)));

        if(variance_reduction){
            iter_delta_vr = (1.0 / MC_iterations) * exp(- risk_free_rate * tau) * (payoff_object.gradient(iter_price_vr).cwiseProduct(iter_price_vr.cwiseQuotient(initial_price)));

            Delta += 0.5 * (iter_delta + iter_delta_vr);

        }else{Delta += iter_delta;}


        // THETA (Adjoint Algorithmic Differentiation) ---------------------------------------------------------

        auto theta_valuation = risk_free_rate * payoff_object(iter_price); 

        auto theta_gradient = payoff_object.gradient(iter_price).transpose();

            auto theta_drift = risk_free_rate_vector - 0.5 * asset_covariance.diagonal(); 

            auto theta_corr_brownian_shock = (iter_price.cwiseQuotient(initial_price)).array().log() - theta_drift.array() * tau;

        auto theta_log_price_time_grad = theta_drift + (theta_corr_brownian_shock / (2 * tau)).matrix();

        auto theta_price_time_sensitivity = iter_price.cwiseProduct(theta_log_price_time_grad);

        iter_theta = (1.0 / MC_iterations) * exp(- risk_free_rate * tau) * (theta_valuation - theta_gradient * theta_price_time_sensitivity);

        if(variance_reduction){
            auto theta_valuation_vr = risk_free_rate * payoff_object(iter_price_vr);

            auto theta_gradient_vr = payoff_object.gradient(iter_price_vr).transpose();

                auto theta_corr_brownian_shock_vr = (iter_price_vr.cwiseQuotient(initial_price)).array().log() - theta_drift.array() * tau;

            auto theta_log_price_time_grad_vr = theta_drift + (theta_corr_brownian_shock_vr / (2 * tau)).matrix();

            auto theta_price_time_sensitivity_vr = iter_price_vr.cwiseProduct(theta_log_price_time_grad_vr);

            iter_theta_vr = (1.0 / MC_iterations) * exp(- risk_free_rate * tau) * (theta_valuation_vr - theta_gradient_vr * theta_price_time_sensitivity_vr);

            Theta += 0.5 * (iter_theta + iter_theta_vr);

        }else{Theta += iter_theta;}

        
        // GAMMA (Finite Differences with Common Random Numbers)---------------------------------------------------------  
        
        auto V_0 = payoff_object(iter_price);
        auto V_0_vr = payoff_object(iter_price_vr);
        
        for(int i = 0; i < M; i++){

            // Diagonal
            auto price_bump_p = (initial_price + e.col(i) * h(i)).cwiseQuotient(initial_price);
            auto price_bump_m = (initial_price - e.col(i) * h(i)).cwiseQuotient(initial_price);
            
            
            auto V_p = payoff_object(iter_price.cwiseProduct(price_bump_p));            
            auto V_m = payoff_object(iter_price.cwiseProduct(price_bump_m));

            iter_gamma(i, i) = (V_p - (2 * V_0) + V_m) / static_cast<double>(h(i) * h(i));

            if(variance_reduction){
                auto V_p_vr = payoff_object(iter_price_vr.cwiseProduct(price_bump_p));            
                auto V_m_vr = payoff_object(iter_price_vr.cwiseProduct(price_bump_m));

                iter_gamma_vr(i, i) = (V_p_vr - (2 * V_0_vr) + V_m_vr) / static_cast<double>(h(i) * h(i));
            }
            
            for(int j = i + 1; j < M; j++){

                //Off-diagonal
                auto price_bump_pp = (initial_price + e.col(i) * h(i) + e.col(j) * h(j)).cwiseQuotient(initial_price);
                auto price_bump_pm = (initial_price + e.col(i) * h(i) - e.col(j) * h(j)).cwiseQuotient(initial_price);
                auto price_bump_mp = (initial_price - e.col(i) * h(i) + e.col(j) * h(j)).cwiseQuotient(initial_price);
                auto price_bump_mm = (initial_price - e.col(i) * h(i) - e.col(j) * h(j)).cwiseQuotient(initial_price);

                auto V_pp = payoff_object(iter_price.cwiseProduct(price_bump_pp));
                auto V_pm = payoff_object(iter_price.cwiseProduct(price_bump_pm));
                auto V_mp = payoff_object(iter_price.cwiseProduct(price_bump_mp));
                auto V_mm = payoff_object(iter_price.cwiseProduct(price_bump_mm));

                iter_gamma(i, j) = (V_pp - V_pm - V_mp + V_mm) / static_cast<double>(4 * h(i) * h(j));            
                iter_gamma(j, i) = iter_gamma(i, j);

                if(variance_reduction){
                    auto V_pp_vr = payoff_object(iter_price_vr.cwiseProduct(price_bump_pp));
                    auto V_pm_vr = payoff_object(iter_price_vr.cwiseProduct(price_bump_pm));
                    auto V_mp_vr = payoff_object(iter_price_vr.cwiseProduct(price_bump_mp));
                    auto V_mm_vr = payoff_object(iter_price_vr.cwiseProduct(price_bump_mm));

                    iter_gamma_vr(i, j) = (V_pp_vr - V_pm_vr - V_mp_vr + V_mm_vr) / static_cast<double>(4 * h(i) * h(j));                
                    iter_gamma_vr(j, i) = iter_gamma_vr(i, j);
                }          
            }
        }

        iter_gamma *= ((1.0 / MC_iterations) * exp(-risk_free_rate * tau));

        if(variance_reduction){            
            iter_gamma_vr *= ((1.0 / MC_iterations) * exp(-risk_free_rate * tau));
            Gamma += 0.5 * (iter_gamma + iter_gamma_vr);
        }else{Gamma += iter_gamma;}
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

    Eigen::MatrixXd xxx(M, D);
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

