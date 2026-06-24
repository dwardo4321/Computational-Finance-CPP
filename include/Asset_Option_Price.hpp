#pragma once

#include <iostream>
#include <Eigen/Dense>
#include <cmath>
#include <numbers>
#include <vector>
#include <random>
#include <optional>
#include <functional>
#include <string>
#include <sstream>

class Asset_Option_Price{
    
    private:

        Eigen::VectorXd strike;     
        Eigen::VectorXd rate;         // fixed rate
        Eigen::VectorXd risk_free_rate;
        Eigen::MatrixXd volatility;   // fixed volatility
        Eigen::VectorXd price_today;
        double Time;                  // time duration
        int dimensions;               // number of assets
        int discretisation_brownian_motion; // number of steps 

        std::pair <Eigen::MatrixXd, Eigen::MatrixXd> GBM_price_path_generator(std::optional<Eigen::MatrixXd> correlation_matrix);

    public:

        // Constructor 1: default constructor (homogeneous multi-assets)--------------------------------------------------------------------------------------------
        Asset_Option_Price(
                double strike_constr,
        double rate_constr,
        double risk_free_rate,
        double volatility_constr,
        double price_today_constr,
        double Time_constr,
        int dimensions_constr,
        int discretisation_brownian_motion_constr);

        // Constructor 2: member initializer list (heterogeneous multi-assets) --------------------------------------------------------------------------------------------
        template <typename Scalar_Vector, typename Vector_Matrix>
        Asset_Option_Price(
                const Scalar_Vector& strike_constr,
                const Scalar_Vector& rate_constr,
                const Scalar_Vector& risk_free_rate_constr,
                const Eigen::MatrixBase<Vector_Matrix>& volatility_constr,
                const Eigen::VectorXd& price_today_constr,
                double Time_constr,
                //int dimensions_constr,
                int discretisation_brownian_motion_constr):

            strike(strike_constr), rate(rate_constr), risk_free_rate(risk_free_rate_constr), volatility(volatility_constr), price_today(price_today_constr),
            Time(Time_constr), dimensions(static_cast<int>(strike_constr.size())), discretisation_brownian_motion(discretisation_brownian_motion_constr) 
            {
                if (discretisation_brownian_motion_constr < 2){
                        throw std::runtime_error("There should be more than 2 steps");
                    }
                
                if (Time_constr < 0){
                        throw std::runtime_error("Time should be greater than 0");
                    }
            }
                
        // Class Method 1 -------------------------------------------------------------------------------------------
        Eigen::MatrixXd Brownian_path_generator(std::optional<Eigen::MatrixXd> correlation_matrix);

        // Class Method 2 -------------------------------------------------------------------------------------------
        Eigen::MatrixXd GBM_price_path(std::optional<Eigen::MatrixXd> correlation_matrix);

        // Class Method 3 -------------------------------------------------------------------------------------------
        struct Option_output
        {
            Eigen::VectorXd sample_mean;
            Eigen::VectorXd sample_variance;
            Eigen::VectorXd sample_standard_error;
            std::string confidence_intervals;
        };

        enum class payoff{basket, maxim, worst_off};
        
        Eigen::MatrixXd  discounted_pay_off_calculator(int number_of_iterations, Eigen::VectorXd risk_free_rate, double tau, bool variance_reduction,
                                                        std::optional<payoff> payoff_calculation, Eigen::VectorXd weights, double strike_basket,
                                                        std::optional<Eigen::MatrixXd> correlation_matrix, std::function < std::pair<Eigen::MatrixXd,
                                                        Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > custom_price_generator = nullptr);

        Option_output Monte_Carlo_option_pricer(int number_of_iterations, Eigen::VectorXd risk_free_rate, double tau, bool variance_reduction,
                                                std::optional<payoff> payoff_calculation, Eigen::VectorXd weights, double strike_basket,
                                                std::optional<Eigen::MatrixXd> correlation_matrix, std::function < std::pair<Eigen::MatrixXd,
                                                Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > custom_price_generator = nullptr);

};
