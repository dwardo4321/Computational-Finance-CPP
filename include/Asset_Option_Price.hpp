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


class Payoff{ //-------------------------------------------------------------------------------------------------------------

    public:

        virtual ~Payoff() = default;

        virtual double operator()(const Eigen::VectorXd& terminal_prices) const = 0;

};

    class Basket_Assets: public Payoff{ //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

        private:

            double strike;
            Eigen::VectorXd weights;
        
        public:
            // Construstor_1: Payoff
            Basket_Assets(double strike_const, Eigen::VectorXd weights_const);
            // Method_1: Payoff
            double operator()(const Eigen::VectorXd& terminal_prices) const override;
    };

    class Maximum_Asset: public Payoff{ //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

        private:

            double strike;
        
        public:
            // Construstor_1: Payoff
            Maximum_Asset(double strike_const);
            // Method_1: Payoff
            double operator()(const Eigen::VectorXd& terminal_prices) const override;
    };

    class Minimum_Asset: public Payoff{ //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
                
        private:

            double strike;
        
        public:
            // Construstor_1: Payoff
            Minimum_Asset(double strike_const);
            // Method_1: Payoff
            double operator()(const Eigen::VectorXd& terminal_prices) const override;
    }; 

class Asset_Option_Price{ //-------------------------------------------------------------------------------------------------------------
    
    protected:

   
        Eigen::VectorXd rate;         // fixed rate

        Eigen::MatrixXd volatility;   // fixed volatility
        Eigen::VectorXd price_today;
        double strike;  
        double risk_free_rate;
        double Time;                  // time duration
        int dimensions;               // number of assets
        int discretisation_brownian_motion; // number of steps 
         

        // Private Method 1 //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
        Eigen::MatrixXd Brownian_path_generator(std::optional<Eigen::MatrixXd> correlation_matrix);

        // Private Method 2 //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
        std::pair <Eigen::MatrixXd, Eigen::MatrixXd> GBM_price_path_generator(std::optional<Eigen::MatrixXd> correlation_matrix);
        
    public:

        // Constructor 1: default constructor (homogeneous multi-assets) -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
        Asset_Option_Price(
                double strike_constr,
                double rate_constr,
                double risk_free_rate,
                double volatility_constr,
                double price_today_constr,
                double Time_constr,
                int discretisation_brownian_motion_constr);
        
        // Constructor 2: member initializer list (heterogeneous multi-assets) -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
        Asset_Option_Price(
            const double& strike_constr,
            const Eigen::VectorXd& rate_constr,
            const double& risk_free_rate_constr,
            const Eigen::MatrixXd& volatility_constr,
            const Eigen::VectorXd& price_today_constr,
            double Time_constr,
            //int dimensions_constr,
            int discretisation_brownian_motion_constr);

        // Constructor 3: member initializer list (heterogeneous multi-assets) -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
        Asset_Option_Price(
            const double& strike_constr,
            const Eigen::VectorXd& rate_constr,
            const double& risk_free_rate_constr,
            const Eigen::VectorXd& volatility_constr,
            const Eigen::VectorXd& price_today_constr,
            double Time_constr,
            //int dimensions_constr,
            int discretisation_brownian_motion_constr);

        // Class Method 1 //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
        Eigen::MatrixXd GBM_price_path(std::optional<Eigen::MatrixXd> correlation_matrix);

        // Class Method 2 //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*    
        Eigen::VectorXd discounted_pay_off_calculator(int number_of_iterations, double risk_free_rate, double tau, bool variance_reduction,
                                                    std::optional<Eigen::MatrixXd> correlation_matrix,
                                                    const Payoff& payoff_object,
                                                    std::function < std::pair<Eigen::MatrixXd, Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > custom_price_generator);
        
        // Class Method 3 //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
        struct Option_output{
            double sample_mean;
            double sample_variance;
            double sample_standard_error;
            std::string confidence_intervals;
        };

        Option_output Monte_Carlo_option_pricer(int number_of_iterations, double risk_free_rate, double tau, bool variance_reduction,
                                                std::optional<Eigen::MatrixXd> correlation_matrix,
                                                const Payoff& payoff_object,
                                                std::function < std::pair<Eigen::MatrixXd, Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > custom_price_generator);

};

