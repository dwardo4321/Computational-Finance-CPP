#pragma once

#include <Eigen/Dense>
#include <Eigen/Core>
#include <iostream>
#include <optional> 
#include <functional>

class Multidimensional_Risk_Neutral_Engine{
    
    private:

        Eigen::VectorXd strike; 
        double strike_one_der;    
        Eigen::VectorXd rate;         // fixed rates
        Eigen::VectorXd risk_free_rate;
        double risk_free_rate_one_der;
        Eigen::VectorXd price_today;

        Eigen::MatrixXd volatility_realised;   // assets volatility
        Eigen::MatrixXd volatility_implied;   // option volatilities
        Eigen::MatrixXd correlation_matrix;
        
        double Time;        // time duration
        int discretisation; // number of steps 

        std::pair <Eigen::MatrixXd, Eigen::MatrixXd> Multidimensional_GBM(double tau, int discretisation, std::optional<Eigen::MatrixXd> correlation_matrix, Eigen::VectorXd initial_price);
    
    public:

        Multidimensional_Risk_Neutral_Engine(Eigen::VectorXd strike_const, Eigen::VectorXd rate_const, Eigen::VectorXd risk_free_rate_const, Eigen::VectorXd price_today_const,
                                             Eigen::MatrixXd volatility_realised_const, Eigen::MatrixXd volatility_implied_const, 
                                             double Time_const, int discretisation_const);

        struct quad{Eigen::VectorXd Delta; Eigen::MatrixXd Gamma; Eigen::VectorXd Theta; Eigen::MatrixXd Option;};
        
        quad Greeks_and_Option(int MC_iterations, double tau, double price_change, bool variance_reduction, Eigen::VectorXd initial_price, std::optional<Eigen::MatrixXd> correlation_matrix);
             
        //Eigen::VectorXd Portfolio(int discretisation, double tau, Eigen::VectorXd initial_price); 
        
        Eigen::MatrixXd Risk_Neutral_MultiDim_DHE(bool call);
};

