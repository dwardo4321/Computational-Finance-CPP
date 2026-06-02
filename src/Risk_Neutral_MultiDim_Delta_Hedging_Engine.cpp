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

Eigen::MatrixXd Multidimensional_Risk_Neutral_Engine::Brownian_Mot(int discretisation, double Time, std::optional<Eigen::MatrixXd> correlation_matrix){

    int dimensions = volatility_realised.cols();

    Eigen::MatrixXd out = utility::Brownian_path_generator(discretisation, dimensions, Time, std::nullopt);

    return out;
}

// Private Method 2 ---------------------------------------------------------
Eigen::MatrixXd Multidimensional_Risk_Neutral_Engine::Multidimensional_GBM(std::optional<Eigen::MatrixXd> correlation_matrix){

    Eigen::MatrixXd out; 

    int M = volatility_realised.rows();
    int D = volatility_realised.cols();

    /*
    for(int i; i < M; i++){

        M * D;        
    }
    */

    return out;
}

// Public Method 1 ---------------------------------------------------------
Eigen::MatrixXd Multidimensional_Risk_Neutral_Engine::Risk_Neutral_MultiDim_DHE(bool call){
    
    Eigen::MatrixXd out = Brownian_Mot(discretisation, Time, std::nullopt); 

    return out; 
}
