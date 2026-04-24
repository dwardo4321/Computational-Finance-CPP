#pragma once

#include <iostream>
#include <Eigen/Dense>
#include <cmath>
#include <numbers>
#include <vector>
#include <random>
#include <optional>
#include <string>
#include <sstream>

class Asset_Option_Price{
    
    private:

        Eigen::VectorXd strike;     
        Eigen::VectorXd rate;         // fixed rate
        Eigen::VectorXd volatility;   // fixed volatility
        Eigen::VectorXd price_today;
        double Time;                  // time duration
        int dimensions;               // number of assets
        int discretisation_brownian_motion; // number of steps 

        std::pair <Eigen::MatrixXd, Eigen::MatrixXd> GBM_price_path_generator(std::optional<Eigen::MatrixXd> correlation_matrix);
        
        Eigen::MatrixXd discounted_pay_off_calculator(int number_of_iterations, bool variance_reduction, std::optional<Eigen::MatrixXd> correlation_matrix);

    public:

        // Constructor 1: default constructor (homogeneous multi-assets)--------------------------------------------------------------------------------------------
        Asset_Option_Price(
                double strike_constr,
                double rate_constr,
                double volatility_constr,
                double price_today_constr,
                double Time_constr,
                int dimensions_constr,
                int discretisation_brownian_motion_constr);

        // Constructor 2: member initializer list (heterogeneous multi-assets) --------------------------------------------------------------------------------------------
        Asset_Option_Price(
                const Eigen::VectorXd& strike_constr,
                const Eigen::VectorXd& rate_constr,
                const Eigen::VectorXd& volatility_constr,
                const Eigen::VectorXd& price_today_constr,
                double Time_constr,
                //int dimensions_constr,
                int discretisation_brownian_motion_constr);
                
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

        Option_output Monte_Carlo_option_pricer(int number_of_iterations , bool variance_reduction, std::optional<Eigen::MatrixXd> correlation_matrix);

};
