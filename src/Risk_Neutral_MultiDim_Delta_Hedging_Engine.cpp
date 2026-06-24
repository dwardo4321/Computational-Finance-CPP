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

namespace{
    std::random_device rd;
    std::mt19937_64 gen(rd());
}

using namespace Eigen::placeholders;

// Constructor ---------------------------------------------------------
Multidimensional_Risk_Neutral_Engine::Multidimensional_Risk_Neutral_Engine(Eigen::VectorXd strike_const, Eigen::VectorXd rate_const, Eigen::VectorXd risk_free_rate_const, Eigen::VectorXd price_today_const,
                                                                            Eigen::MatrixXd volatility_realised_const, Eigen::MatrixXd volatility_implied_const, 
                                                                            double Time_const, int discretisation_const):

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
std::pair <Eigen::MatrixXd, Eigen::MatrixXd> Multidimensional_Risk_Neutral_Engine::Multidimensional_GBM(double tau, int discretisation, std::optional<Eigen::MatrixXd> correlation_matrix, Eigen::VectorXd initial_price){

    int M = volatility_realised.rows();
    int D = volatility_realised.cols();
    int dimensions = volatility_realised.cols();

    Eigen::MatrixXd brownian_mot = utility::Brownian_path_generator(discretisation, dimensions, tau, correlation_matrix);

    Eigen::VectorXd risk_premium = rate - risk_free_rate;

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

        change_in_price.row(t) = (risk_free_rate.transpose().array() * price.row(t-1).array() * dt) + (price.row(t-1).array() * vol.row(t).array());
        change_in_price_vd.row(t) = (risk_free_rate.transpose().array() * price_variance_reduction.row(t-1).array() * dt) + (price_variance_reduction.row(t-1).array() * vol_vd.row(t).array());
        
        price.row(t) = price.row(t-1) + change_in_price.row(t);
        price_variance_reduction.row(t) = price_variance_reduction.row(t-1) + change_in_price_vd.row(t);
    }
   
    return {price, price_variance_reduction};
}

// Bump and Run Method
Multidimensional_Risk_Neutral_Engine::quad Multidimensional_Risk_Neutral_Engine::Greeks_and_Option(int MC_iterations, double tau, double price_change, bool variance_reduction, Eigen::VectorXd initial_price, std::optional<Eigen::MatrixXd> correlation_matrix){

    /* Eigen::VectorXd initial_price_up = initial_price.array() + price_change;
    Eigen::VectorXd initial_price_down = initial_price.array() - price_change;
    
    Asset_Option_Price Option_value_up = Asset_Option_Price(strike, risk_free_rate, volatility_realised, initial_price_up, tau, discretisation);
    Asset_Option_Price Option_value_down = Asset_Option_Price(strike, risk_free_rate, volatility_realised, initial_price_down, tau, discretisation); 
    Asset_Option_Price Option_value = Asset_Option_Price(strike, risk_free_rate, volatility_realised, initial_price, tau, discretisation);

    auto custom_func = [this, tau] (std::optional<Eigen::MatrixXd> correlation_matrix){return this-> Multidimensional_GBM(tau, this->discretisation, correlation_matrix, this->price_today);};

    Eigen::VectorXd Option_up = Option_value_up.Monte_Carlo_option_pricer(MC_iterations, tau, variance_reduction, correlation_matrix, custom_func).sample_mean;
    Eigen::VectorXd Option_down = Option_value_down.Monte_Carlo_option_pricer(MC_iterations, tau, variance_reduction, correlation_matrix, custom_func).sample_mean;
    Eigen::VectorXd Option = Option_value.Monte_Carlo_option_pricer(MC_iterations, tau, variance_reduction, correlation_matrix, custom_func).sample_mean;

    Eigen::VectorXd Delta = (Option_up - Option_down) / (2 * price_change);

    Eigen::VectorXd Gamma = (Option_up - 2 * )





    return {Delta, Gamma, Theta, Option_value}; */
}

/* Eigen::VectorXd Portfolio(int discretisation, double tau, Eigen::VectorXd initial_price){

    Eigen::VectorXd portfolio_value;

    auto[price, price_vd] = Multidimensional_GBM(tau, discretisation, correlation_matrix, initial_price);

    double portfolio_value = delta * price + bank;

} */


// Public Method 1 ---------------------------------------------------------
Eigen::MatrixXd Multidimensional_Risk_Neutral_Engine::Risk_Neutral_MultiDim_DHE(bool call){
    
    /* Eigen::VectorXd out = Z_scores(true);//Brownian_Mot(discretisation, Time, std::nullopt); 

    Eigen::MatrixXd xxx(volatility_implied.rows(), volatility_implied.cols());
    xxx.col(0) = out;
    xxx.col(1) = out;
    auto[A, B] = Multidimensional_GBM(std::nullopt); */

    double dt = Time / (discretisation - 1);

    double tau = Time - 10 * dt;

    Asset_Option_Price Option_value = Asset_Option_Price(strike, risk_free_rate, volatility_realised, price_today, tau, discretisation);

    auto custom_func = [this, tau] (std::optional<Eigen::MatrixXd> correlation_matrix){return this-> Multidimensional_GBM(tau, this->discretisation, correlation_matrix, this->price_today);};

    Eigen::VectorXd weights;
    weights << 0.3, 0.3, 0.2, 0.1, 0.05;
    
    Eigen::VectorXd out = Option_value.Monte_Carlo_option_pricer(1000, 0.15, tau, true, basket.value(), weights, strike_one_der, std::nullopt, custom_func).sample_mean; 

    Eigen::MatrixXd xxx(out.size(), 2);
    xxx.col(0) = out;
    xxx.col(1) = out;

    return xxx;
}

