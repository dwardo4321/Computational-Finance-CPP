#pragma once

#include <Eigen/Dense>
#include <iostream>
#include <optional> 

class Multidimensional_Risk_Neutral_Engine{
    
    private:

        Eigen::VectorXd strike;     
        Eigen::VectorXd rate;         // fixed rates
        Eigen::VectorXd price_today;

        Eigen::MatrixXd volatility_realised;   // assets volatility
        Eigen::MatrixXd volatility_implied;   // option volatilities
        Eigen::MatrixXd correlation_matrix;
        
        double Time;        // time duration
        int discretisation; // number of steps 

        Eigen::MatrixXd Multidimensional_GBM(std::optional<Eigen::MatrixXd> correlation_matrix);
    
    public:

        Multidimensional_Risk_Neutral_Engine(Eigen::VectorXd strike_const, Eigen::VectorXd rate_const, Eigen::VectorXd price_today_const,
                                             Eigen::MatrixXd volatility_realised_const, Eigen::MatrixXd volatility_implied_const, 
                                             double Time_const, int discretisation_const);
       
        Eigen::MatrixXd Risk_Neutral_MultiDim_DHE(bool call);
};

