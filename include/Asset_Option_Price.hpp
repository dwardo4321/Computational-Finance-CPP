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
class Asset_Option_Price{ //-------------------------------------------------------------------------------------------------------------
    
    protected:

        Eigen::VectorXd strike;     
        Eigen::VectorXd rate;         // fixed rate
        Eigen::VectorXd risk_free_rate;
        Eigen::MatrixXd volatility;   // fixed volatility
        Eigen::VectorXd price_today;
        double Time;                  // time duration
        int dimensions;               // number of assets
        int discretisation_brownian_motion; // number of steps 
         

        // Private Method 1 //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
        Eigen::MatrixXd Brownian_path_generator(std::optional<Eigen::MatrixXd> correlation_matrix);
        
        // Private Method 2 //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
        std::pair <Eigen::MatrixXd, Eigen::MatrixXd> GBM_price_path_generator(std::optional<Eigen::MatrixXd> correlation_matrix);

        // Private Method 3 //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
        Eigen::MatrixXd GBM_price_path(std::optional<Eigen::MatrixXd> correlation_matrix);

    public:

        // Constructor 1: default constructor (homogeneous multi-assets) -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
        Asset_Option_Price(
                double strike_constr,
                double rate_constr,
                double risk_free_rate,
                double volatility_constr,
                double price_today_constr,
                double Time_constr,
                int dimensions_constr,
                int discretisation_brownian_motion_constr);
        
        // Constructor 2: member initializer list (heterogeneous multi-assets) -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
        Asset_Option_Price(
                const Eigen::VectorXd& strike_constr,
                const Eigen::VectorXd& rate_constr,
                const Eigen::VectorXd& risk_free_rate_constr,
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
            const Eigen::MatrixXd& volatility_constr,
            const Eigen::VectorXd& price_today_constr,
            double Time_constr,
            //int dimensions_constr,
            int discretisation_brownian_motion_constr);

        // Constructor 4: member initializer list (heterogeneous multi-assets) -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
        Asset_Option_Price(
            const double& strike_constr,
            const Eigen::VectorXd& rate_constr,
            const double& risk_free_rate_constr,
            const Eigen::VectorXd& volatility_constr,
            const Eigen::VectorXd& price_today_constr,
            double Time_constr,
            //int dimensions_constr,
            int discretisation_brownian_motion_constr);
            
        // Constructor 5: member initializer list (heterogeneous multi-assets) -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
        Asset_Option_Price(
            const Eigen::VectorXd& strike_constr,
            const Eigen::VectorXd& rate_constr,
            const Eigen::VectorXd& risk_free_rate_constr,
            const Eigen::VectorXd& volatility_constr,
            const Eigen::VectorXd& price_today_constr,
            double Time_constr,
            //int dimensions_constr,
            int discretisation_brownian_motion_constr);
        
        // Class Method 4 //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
        struct Option_output{
            Eigen::VectorXd sample_mean;
            Eigen::VectorXd sample_variance;
            Eigen::VectorXd sample_standard_error;
            std::string confidence_intervals;
        };

        enum class payoff{basket, maxim, worst_off}; 
        Option_output Monte_Carlo_option_pricer(int number_of_iterations, Eigen::VectorXd risk_free_rate, double tau, bool variance_reduction,
                                                std::optional<payoff> payoff_calculation, std::optional<Eigen::VectorXd> weights, Eigen::VectorXd strike,
                                                std::optional<Eigen::MatrixXd> correlation_matrix,
                                                std::function < std::pair<Eigen::MatrixXd, Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > custom_price_generator = nullptr);

};

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
    }; //-------------------------------------------------------------------------------------------------------------
