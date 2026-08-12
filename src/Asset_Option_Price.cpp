#include "Asset_Option_Price.hpp"

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
#include <algorithm>

// Constructors and Methods Payoff Class--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// No_1: constructor and method -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Basket_Assets::Basket_Assets(double strike_const, Eigen::VectorXd weights_const): 
                            strike(strike_const), weights(weights_const){};

double Basket_Assets::operator()(const Eigen::VectorXd& terminal_prices) const {
                        double weighted_prices = weights.dot(terminal_prices);
                        double pay_off = std::max(0.0, weighted_prices - strike);
                        return pay_off;
                    };

Eigen::VectorXd Basket_Assets::gradient(const Eigen::VectorXd& terminal_prices) const {
                        Eigen::VectorXd grad;
                        double weighted_prices = weights.dot(terminal_prices);
                        weighted_prices > strike? grad = weights: grad = Eigen::VectorXd::Zero(weights.size()); 
                        return grad;
                    };

// No_2: constructor and method -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Maximum_Asset::Maximum_Asset(double strike_const): strike(strike_const){};

double Maximum_Asset::operator()(const Eigen::VectorXd& terminal_prices) const {
                        double most_price = terminal_prices.maxCoeff();
                        double pay_off = std::max(0.0, most_price - strike);
                        return pay_off;
                    };

Eigen::VectorXd Maximum_Asset::gradient(const Eigen::VectorXd& terminal_prices) const {
                        Eigen::VectorXd grad;
                        double most_price = terminal_prices.maxCoeff();
                        most_price > strike? grad = Eigen::VectorXd::Ones(terminal_prices.size()): grad = Eigen::VectorXd::Zero(terminal_prices.size()); 
                        return grad;
                    };                    

// No_3: constructor and method -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Minimum_Asset::Minimum_Asset(double strike_const): strike(strike_const){};

double Minimum_Asset::operator()(const Eigen::VectorXd& terminal_prices) const {
                        double least_price = terminal_prices.minCoeff();
                        double pay_off = std::max(0.0, least_price - strike);
                        return pay_off;
                    };

Eigen::VectorXd Minimum_Asset::gradient(const Eigen::VectorXd& terminal_prices) const {
                        Eigen::VectorXd grad;
                        double least_price = terminal_prices.minCoeff();
                        least_price > strike? grad = Eigen::VectorXd::Ones(terminal_prices.size()): grad = Eigen::VectorXd::Zero(terminal_prices.size()); 
                        return grad;
                    }; 





                    
// Constructors and Methods Asset_Option_Price Class--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
using namespace Eigen::placeholders;
namespace{std::random_device rd;
          std::mt19937_64 gen1(rd());} 

// Constructor 1: default constructor (homogeneous multi-assets-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Asset_Option_Price::Asset_Option_Price(
        double strike_constr,
        double rate_constr,
        double risk_free_rate_constr,
        double volatility_constr,
        double price_today_constr,
        double Time_constr,
        int discretisation_brownian_motion_constr):

        strike(strike_constr), rate(Eigen::VectorXd::Constant(1, rate_constr)), risk_free_rate(risk_free_rate_constr), volatility(Eigen::VectorXd::Constant(1, volatility_constr)),
        price_today(Eigen::VectorXd::Constant(1, price_today_constr)), Time(Time_constr), dimensions(static_cast<int>(1)), discretisation_brownian_motion(discretisation_brownian_motion_constr){}

// Constructor 2 -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Asset_Option_Price::Asset_Option_Price(
        const double& strike_constr,
        const Eigen::VectorXd& rate_constr,
        const double& risk_free_rate_constr,
        const Eigen::MatrixXd& volatility_constr,
        const Eigen::VectorXd& price_today_constr,
        double Time_constr,
        //int dimensions_constr,
        int discretisation_brownian_motion_constr):

        strike(strike_constr), rate(rate_constr), risk_free_rate(risk_free_rate_constr), volatility(volatility_constr), price_today(price_today_constr),
        Time(Time_constr), dimensions(static_cast<int>(rate_constr.size())), discretisation_brownian_motion(discretisation_brownian_motion_constr) 
        {
            if (discretisation_brownian_motion_constr < 2){
                    throw std::runtime_error("There should be more than 2 steps");
                }
            
            if (Time_constr < 0){
                    throw std::runtime_error("Time should be greater than 0");
                }
        }

// Constructor 3: member initializer list (heterogeneous multi-assets) -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Asset_Option_Price::Asset_Option_Price(
            const double& strike_constr,
            const Eigen::VectorXd& rate_constr,
            const double& risk_free_rate_constr,
            const Eigen::VectorXd& volatility_constr,
            const Eigen::VectorXd& price_today_constr,
            double Time_constr,
            //int dimensions_constr,
            int discretisation_brownian_motion_constr):

        strike(strike_constr), rate(rate_constr), risk_free_rate(risk_free_rate_constr), volatility(volatility_constr), price_today(price_today_constr),
        Time(Time_constr), dimensions(static_cast<int>(rate_constr.size())), discretisation_brownian_motion(discretisation_brownian_motion_constr) 
        {
            if (discretisation_brownian_motion_constr < 2){
                    throw std::runtime_error("There should be more than 2 steps");
                }
            
            if (Time_constr < 0){
                    throw std::runtime_error("Time should be greater than 0");
                }
        }

// Private Method 1 -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Eigen::MatrixXd  Asset_Option_Price::Brownian_path_generator(std::optional<Eigen::MatrixXd> correlation_matrix){
    
    Eigen::MatrixXd paths(discretisation_brownian_motion, dimensions);
    paths.row(0).setZero();

    double delta_time = static_cast<double>(Time) / (discretisation_brownian_motion - 1);
    
    // Generate Z ~ N(0, I_(nxn))   
    std::normal_distribution<double> standard_norm(0, 1);
    Eigen::MatrixXd standard_normal_rv(discretisation_brownian_motion, dimensions);
    for (int i = 0; i < dimensions; i++)
    {
        for(int j = 0; j < discretisation_brownian_motion; j++)
        {
            standard_normal_rv(j, i) = standard_norm(gen1);
        }                 
    }

    if(correlation_matrix.has_value()){

        // Cholesky Decomposition
        Eigen::LLT<Eigen::MatrixXd> llt(correlation_matrix.value());
        Eigen::MatrixXd lower_triangular = llt.matrixL();
        
        // dW(t), and BM (Brownian Motion) 
        Eigen::VectorXd delta_BM;
    
        for(int i = 1; i < discretisation_brownian_motion; i++)
        {
            delta_BM = sqrt(delta_time) * (lower_triangular * standard_normal_rv.row(i).transpose());
            paths.row(i) = paths.row(i - 1) + delta_BM.transpose();
        }

    } 
    else
    {
        for(int i = 1; i < discretisation_brownian_motion; i++)
        {
            paths.row(i) = paths.row(i - 1) + standard_normal_rv.row(i) * sqrt(delta_time);
        }
    }

    return paths; 
};

// Private Method 2 -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
std::pair <Eigen::MatrixXd, Eigen::MatrixXd> Asset_Option_Price::GBM_price_path_generator(std::optional<Eigen::MatrixXd> correlation_matrix){ 
    // Pass-by-Reference (additional matrix prices_paths_variance_reduction)
    Eigen::MatrixXd brownian_paths(discretisation_brownian_motion, dimensions);
    correlation_matrix.has_value()? brownian_paths = Brownian_path_generator(correlation_matrix.value()): brownian_paths = Brownian_path_generator(std::nullopt);
    
    Eigen::MatrixXd prices_paths(discretisation_brownian_motion, dimensions);
    Eigen::MatrixXd prices_paths_variance_reduction(discretisation_brownian_motion, dimensions);
    
    prices_paths.row(0) = price_today.transpose();
    prices_paths_variance_reduction.row(0) = price_today.transpose();

    //Eigen::VectorXd time_steps = Eigen::VectorXd::LinSpaced(discretisation_brownian_motion, 0, Time);

    

    double delta_time = static_cast<double>(Time) / (discretisation_brownian_motion - 1);

    for (int i = 0; i < dimensions; i++){  

        for (int j = 1; j < discretisation_brownian_motion; j++){ 

            double delta_BM = brownian_paths(j, i) - brownian_paths(j - 1, i);

            double X = volatility(i, 0) * delta_BM + (rate(i) - 0.5*pow(volatility(i, 0), 2)) * delta_time;
            double X_vd = volatility(i, 0) * -delta_BM + (rate(i) - 0.5*pow(volatility(i, 0), 2)) * delta_time;

            prices_paths(j, i) = prices_paths(j - 1, i) * exp(X);   
            prices_paths_variance_reduction(j, i) = prices_paths_variance_reduction(j - 1, i) * exp(X_vd);               
        }
    }
    return {prices_paths, prices_paths_variance_reduction};
}
 
// Class Method 1 -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Eigen::MatrixXd  Asset_Option_Price::GBM_price_path(std::optional<Eigen::MatrixXd> correlation_matrix){
    Eigen::MatrixXd out(discretisation_brownian_motion, dimensions);
    if(correlation_matrix.has_value()){
        auto[A, B] = GBM_price_path_generator(*correlation_matrix);
        return A;
    }else{
        auto[A, B] = GBM_price_path_generator(std::nullopt);
        return A;
    }    
}

// Class Method 2 -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Eigen::VectorXd  Asset_Option_Price::discounted_pay_off_calculator(int number_of_iterations, double risk_free_rate, double tau, bool variance_reduction,
                                                                    std::optional<Eigen::MatrixXd> correlation_matrix,
                                                                    const Payoff& payoff_object,
                                                                    std::function < std::pair<Eigen::MatrixXd, Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > custom_price_generator){
                                                                 
    Eigen::MatrixXd prices(number_of_iterations, dimensions);
    Eigen::MatrixXd prices_variance_reduction(number_of_iterations, dimensions);

    Eigen::VectorXd pay_off(number_of_iterations);
    Eigen::VectorXd pay_off_variance_reduction(number_of_iterations);  

    Eigen::VectorXd discounted_pay_off(number_of_iterations);

    std::function < std::pair<Eigen::MatrixXd, Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > price_generator;

    if (custom_price_generator){ 

        price_generator = custom_price_generator;
    }else{
        
        price_generator = [this](std::optional <Eigen::MatrixXd> correlation_matrix){

            return this-> GBM_price_path_generator(correlation_matrix);
        };
    }

    for (int i = 0; i < number_of_iterations; i++){
                    
                    Eigen::MatrixXd A, B; 
                    correlation_matrix.has_value()? std::tie(A, B) = price_generator(*correlation_matrix): std::tie(A, B) = price_generator(std::nullopt);
                    prices.row(i) = A(last, all);
                    prices_variance_reduction.row(i) = B(last, all);

                    pay_off(i) = payoff_object(prices.row(i).transpose()) * exp(-risk_free_rate * tau);
                    pay_off_variance_reduction(i) = 0.5 * (pay_off(i) + (payoff_object(prices_variance_reduction.row(i).transpose()) * exp(-risk_free_rate * tau)));
    }

    variance_reduction? discounted_pay_off = pay_off_variance_reduction: discounted_pay_off = pay_off;

    return discounted_pay_off;
}

// Class Method 3 -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Asset_Option_Price::Option_output  Asset_Option_Price::Monte_Carlo_option_pricer(int number_of_iterations, double risk_free_rate, double tau, bool variance_reduction,
                                                                                std::optional<Eigen::MatrixXd> correlation_matrix,
                                                                                const Payoff& payoff_object,
                                                                                std::function < std::pair<Eigen::MatrixXd, Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > custom_price_generator){
        
    std::function < std::pair<Eigen::MatrixXd, Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > price_generator;

    if (custom_price_generator){ 

        price_generator = custom_price_generator;
    }else{
        
        price_generator = [this](std::optional <Eigen::MatrixXd> correlation_matrix){

            return this -> GBM_price_path_generator(correlation_matrix);
        };
    }
    
    Option_output output{};

    Eigen::MatrixXd discounted_pay_off(number_of_iterations, dimensions);
        
    if (variance_reduction){
        correlation_matrix.has_value()? discounted_pay_off = discounted_pay_off_calculator(number_of_iterations, risk_free_rate, tau, true,
                                                                                            *correlation_matrix, payoff_object, price_generator):
                                        discounted_pay_off = discounted_pay_off_calculator(number_of_iterations, risk_free_rate, tau, true,
                                                                                            std::nullopt, payoff_object, price_generator);
    }else{
        correlation_matrix.has_value()? discounted_pay_off = discounted_pay_off_calculator(number_of_iterations, risk_free_rate, tau, false,
                                                                                            *correlation_matrix, payoff_object, price_generator):    
                                        discounted_pay_off = discounted_pay_off_calculator(number_of_iterations, risk_free_rate, tau, false,
                                                                                            std::nullopt, payoff_object, price_generator);
    }
    
    double sample_mean;
    double sample_variance;
    double sample_standard_error;
    double ci_lower_bound;
    double ci_upper_bound;
    
    
    // SAMPLE MEAN -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    sample_mean = discounted_pay_off.mean();
    output.sample_mean = sample_mean;  
    
    // SAMPLE VARIANCE -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    for (int i = 0; i < number_of_iterations; i++){
        sample_variance += pow((discounted_pay_off(i) - sample_mean), 2);    
    }
    sample_variance = sample_variance / static_cast<double>(number_of_iterations - 1);
    output.sample_variance = sample_variance;
    
    // STANDARD ERROR -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    sample_standard_error = sqrt(sample_variance / static_cast<double>(number_of_iterations));
    output.sample_standard_error = sample_standard_error;

    // CONFIDENCE INTERVALS -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    std::stringstream confidence_intervals;

    ci_lower_bound = sample_mean - 1.96 * sample_standard_error;   
    ci_upper_bound = sample_mean + 1.96 * sample_standard_error;
    confidence_intervals << "[" << ci_lower_bound << ": " << ci_upper_bound << "]\n";

    output.confidence_intervals = confidence_intervals.str();
    return output;
}



                                          