#pragma once

#include <Eigen/Dense>
#include <Eigen/Core>
#include <iostream>
#include <optional> 
#include <functional>

class Multidimensional_Risk_Neutral_Engine{
    
    private:
        
        Eigen::VectorXd strike; 
        Eigen::VectorXd rate;         // fixed rates
        Eigen::VectorXd risk_free_rate;
        Eigen::VectorXd price_today;

        Eigen::MatrixXd volatility_realised;   // assets volatility
        Eigen::MatrixXd volatility_implied;   // option volatilities
        Eigen::MatrixXd correlation_matrix;
        
        double Time;        // time duration
        int discretisation; // number of steps 

        std::pair <Eigen::MatrixXd, Eigen::MatrixXd> Multidimensional_GBM(double tau, int discretisation, std::optional<Eigen::MatrixXd> correlation_matrix, Eigen::VectorXd initial_price);

        static Eigen::VectorXd to_vector(double x, int n){return Eigen::VectorXd::Constant(n, x);}

        static Eigen::VectorXd to_vector(const Eigen::VectorXd& x, int n){return x;}
    
    public:

        // Constructor ----------------------------------------------------------------------------
        template <typename Scalar_Vector1, typename Scalar_Vector2>
        Multidimensional_Risk_Neutral_Engine(const Scalar_Vector1& strike_const, 
                                            const Eigen::VectorXd& rate_const, 
                                            const Scalar_Vector2& risk_free_rate_const, 
                                            const Eigen::VectorXd& price_today_const,
                                                                                                                                                   
                                            const Eigen::MatrixXd& volatility_realised_const, 
                                            const Eigen::MatrixXd& volatility_implied_const, 
                                                                                                                                                    
                                            const double& Time_const,
                                            const int& discretisation_const):

                                                                            strike(to_vector(strike_const, price_today_const.size())),  
                                                                            rate(rate_const),         // fixed rates
                                                                            risk_free_rate(to_vector(risk_free_rate_const, price_today_const.size())),
                                                                            price_today(price_today_const),

                                                                            volatility_realised(volatility_realised_const),   // assets volatility
                                                                            volatility_implied(volatility_implied_const),  // option volatilities
                                                                            
                                                                            Time(Time_const),                  // time duration
                                                                            discretisation(discretisation_const)
                                                                            {} // number of steps 

        struct quad{Eigen::VectorXd Delta; Eigen::MatrixXd Gamma; Eigen::VectorXd Theta; Eigen::MatrixXd Option;};
        
        //quad Greeks_and_Option(int MC_iterations, double tau, double price_change, bool variance_reduction, Eigen::VectorXd initial_price, std::optional<Eigen::MatrixXd> correlation_matrix);
             
        //Eigen::VectorXd Portfolio(int discretisation, double tau, Eigen::VectorXd initial_price); 
        
        Eigen::MatrixXd Risk_Neutral_MultiDim_DHE(bool call);
};

