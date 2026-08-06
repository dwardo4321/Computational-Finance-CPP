#pragma once

#include <Eigen/Dense>
#include <Eigen/Core>
#include <iostream>
#include <optional> 
#include <functional>

class Payoff;
class Multidimensional_Risk_Neutral_Engine{
    
    protected:
        
        double strike; 
        Eigen::VectorXd rate;         // fixed rates
        double risk_free_rate;
        Eigen::VectorXd price_today;

        Eigen::MatrixXd volatility_realised;   // assets volatility
        Eigen::MatrixXd volatility_implied;   // option volatilities
        Eigen::MatrixXd correlation_matrix;
        
        double Time;        // time duration
        int discretisation; // number of steps 

        std::pair <Eigen::MatrixXd, Eigen::MatrixXd> Multidimensional_GBM(double tau, int discretisation, std::optional<Eigen::MatrixXd> correlation_matrix, Eigen::VectorXd initial_price);

        static Eigen::VectorXd to_vector(double x, int n){return Eigen::VectorXd::Constant(n, x);}

        static Eigen::VectorXd to_vector(const Eigen::VectorXd& x, int n){return x;}

        //Eigen::VectorXd indicator(Eigen::VectorXd x);
    
    public:

        // Constructor ----------------------------------------------------------------------------
        Multidimensional_Risk_Neutral_Engine(const double& strike_const, 
                                            const Eigen::VectorXd& rate_const, 
                                            const double& risk_free_rate_const, 
                                            const Eigen::VectorXd& price_today_const,
                                                                                                                                                   
                                            const Eigen::MatrixXd& volatility_realised_const, 
                                            const Eigen::MatrixXd& volatility_implied_const, 
                                                                                                                                                    
                                            const double& Time_const,
                                            const int& discretisation_const);

        struct quad{Eigen::VectorXd Delta; Eigen::MatrixXd Gamma; Eigen::VectorXd Theta; double Option;};
        
        quad Greeks_and_Option(int MC_iterations, double time, bool variance_reduction,
                                Eigen::VectorXd initial_price, std::optional<Eigen::MatrixXd> correlation_matrix,
                                const Payoff& payoff_object,
                                std::function < std::pair<Eigen::MatrixXd, Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > custom_price_generator);
             
        //Eigen::VectorXd Portfolio(int discretisation, double tau, Eigen::VectorXd initial_price); 
        
        Eigen::MatrixXd Risk_Neutral_MultiDim_DHE(bool call);
};

