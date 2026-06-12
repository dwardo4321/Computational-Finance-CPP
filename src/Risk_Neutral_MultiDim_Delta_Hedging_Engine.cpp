#include "Risk_Neutral_MultiDim_Delta_Hedging_Engine.hpp"
#include "utilities.hpp"

#include <Eigen/Dense>
#include <boost/math/distributions.hpp>
#include <iostream>
#include <cmath>
#include <random>

namespace{
    std::random_device rd;
    std::mt19937_64 gen(rd());
}


// Constructor ---------------------------------------------------------
Multidimensional_Risk_Neutral_Engine::Multidimensional_Risk_Neutral_Engine(Eigen::VectorXd strike_const, Eigen::VectorXd rate_const, Eigen::VectorXd price_today_const,
                                                                            Eigen::MatrixXd volatility_realised_const, Eigen::MatrixXd volatility_implied_const, 
                                                                            double Time_const, int discretisation_const):

                                                                            strike(strike_const),     
                                                                            rate(rate_const),         // fixed rates
                                                                            price_today(price_today_const),

                                                                            volatility_realised(volatility_realised_const),   // assets volatility
                                                                            volatility_implied(volatility_implied_const),  // option volatilities
                                                                            
                                                                            Time(Time_const),                  // time duration
                                                                            discretisation(discretisation_const)
                                                                            {} // number of steps 


// Private Method 1 ---------------------------------------------------------                                                                           


// Private Method 2 ---------------------------------------------------------
Eigen::MatrixXd Multidimensional_Risk_Neutral_Engine::Multidimensional_GBM(std::optional<Eigen::MatrixXd> correlation_matrix){

    int M = volatility_realised.rows();
    int D = volatility_realised.cols();
    int dimensions = volatility_realised.cols();

    Eigen::MatrixXd brownian_mot = utility::Brownian_path_generator(discretisation, dimensions, Time, std::nullopt);

    Eigen::MatrixXd brownian_mot.colwise() += ((rate - risk_free_rate) / volatility_realised);
    
    Eigen::MatrixXd price(discretisation, price_today.size());
    price.row(0) = price_today.transpose();

    Eigen::MatrixXd change_in_price = Eigen::MatrixXd::Zero(discretisation, price_today.size());

    Eigen::MatrixXd vol = Eigen::MatrixXd::Zero(discretisation, M);

    double dt = Time / static_cast<double>(discretisation - 1);

    for (int t = 1; t < discretisation; t++){

        for(int j = 0; j < M; j++){

            for (int i = 0; i < D; i++){
                
                vol(t, j) += volatility_realised(j, i) * (brownian_mot(t, i) - brownian_mot(t-1, i));
            } 
        }

        change_in_price.row(t) = (risk_free_rate.transpose().array() * price.row(t-1).array() * dt) + (price.row(t-1).array() * vol.row(t).array());
        
        price.row(t) = price.row(t-1) + change_in_price.row(t);
    }

    return price;
}

// Public Method 1 ---------------------------------------------------------
Eigen::MatrixXd Multidimensional_Risk_Neutral_Engine::Risk_Neutral_MultiDim_DHE(bool call){
    
    Eigen::MatrixXd out = Multidimensional_GBM(std::nullopt);//Brownian_Mot(discretisation, Time, std::nullopt); 

    return out; 
}
