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

using namespace Eigen::placeholders;
namespace{std::random_device rd;
          std::mt19937_64 gen1(rd());} 

// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// Private Method 1 -------------------------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructor 1: default constructor (homogeneous multi-assets)--------------------------------------------------------------------------------------------
Asset_Option_Price::Asset_Option_Price(
        double strike_constr,
        double rate_constr,
        double risk_free_rate_constr,
        double volatility_constr,
        double price_today_constr,
        double Time_constr,
        int dimensions_constr,
        int discretisation_brownian_motion_constr):

        strike(Eigen::VectorXd::Constant(dimensions_constr, strike_constr)), rate(Eigen::VectorXd::Constant(dimensions_constr, rate_constr)),
        risk_free_rate(Eigen::VectorXd::Constant(dimensions_constr, risk_free_rate_constr)), volatility(Eigen::VectorXd::Constant(dimensions_constr, volatility_constr)),
        price_today(Eigen::VectorXd::Constant(dimensions_constr, price_today_constr)), Time(Time_constr), dimensions(dimensions_constr), 
        discretisation_brownian_motion(discretisation_brownian_motion_constr) {}
 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// Class Method 1 ------------------------------------------------------------------------------------------------------------------------------------------
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
 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// Class Method 2 ------------------------------------------------------------------------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// Class Method 3 ------------------------------------------------------------------------------------------------------------------------------------------
Eigen::MatrixXd  Asset_Option_Price::discounted_pay_off_calculator(int number_of_iterations, Eigen::VectorXd risk_free_rate, double tau, bool variance_reduction,
                                                                    std::optional<payoff> payoff_calculation, std::optional<Eigen::VectorXd> weights, Eigen::VectorXd strike,
                                                                    std::optional<Eigen::MatrixXd> correlation_matrix, std::function < std::pair<Eigen::MatrixXd,
                                                                    Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > custom_price_generator){
                                                                 
    Eigen::MatrixXd prices(number_of_iterations, dimensions);
    Eigen::MatrixXd prices_variance_reduction(number_of_iterations, dimensions);

    Eigen::MatrixXd pay_off(number_of_iterations, dimensions);
    Eigen::MatrixXd pay_off_variance_reduction(number_of_iterations, dimensions);  

    Eigen::MatrixXd discounted_pay_off(number_of_iterations, dimensions);

    std::function < std::pair<Eigen::MatrixXd, Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > price_generator;

    if (custom_price_generator){ 

        price_generator = custom_price_generator;
    }else{
        
        price_generator = [this](std::optional <Eigen::MatrixXd> correlation_matrix){

            return this -> GBM_price_path_generator(correlation_matrix);
        };
    }

    if(variance_reduction){

        for (int i = 0; i < number_of_iterations; i++){
                        
                        Eigen::MatrixXd A, B; 
                        correlation_matrix.has_value()? std::tie(A, B) = price_generator(*correlation_matrix): std::tie(A, B) = price_generator(std::nullopt);
                        prices.row(i) = A(last, all);
                        prices_variance_reduction.row(i) = B(last, all);

                        if(payoff_calculation){

                            switch(*payoff_calculation){ // * means if(payoff_calculation) then use what is provided inside payoff_calculation
                                
                                case payoff::basket:
                                    
                                    if(weights.has_value())
                                    {(*weights).transpose() * prices.row(i).transpose() - strike(0) > 0? pay_off(i, 0) = ((*weights).transpose() * prices.row(i).transpose() - strike(0)) * exp(-risk_free_rate(0) * tau):  pay_off(i, 0) = 0;  
                                    (*weights).transpose() * prices_variance_reduction.row(i).transpose() - strike(0) > 0? pay_off_variance_reduction(i, 0) = ((*weights).transpose() * prices_variance_reduction.row(i).transpose() - strike(0)) * exp(-risk_free_rate(0) * tau):  pay_off_variance_reduction(i, 0) = 0; 
                                    discounted_pay_off(i, 0) = 0.5 * (pay_off(i, 0) + pay_off_variance_reduction(i, 0));}
                                    break;  

                                case payoff::maxim:

                                    prices.row(i).maxCoeff() - strike(0) > 0? pay_off(i, 0) = (prices.row(i).maxCoeff() - strike(0)) * exp(-risk_free_rate(0) * tau):  pay_off(i, 0) = 0;  
                                    prices_variance_reduction.row(i).maxCoeff() - strike(0) > 0? pay_off_variance_reduction(i, 0) = (prices_variance_reduction.row(i).maxCoeff() - strike(0)) * exp(-risk_free_rate(0) * tau):  pay_off_variance_reduction(i, 0) = 0; 
                                    discounted_pay_off(i, 0) = 0.5 * (pay_off(i, 0) + pay_off_variance_reduction(i, 0)); 
                                    break;

                                case payoff::worst_off:

                                    prices.row(i).minCoeff() - strike(0) > 0? pay_off(i, 0) = (prices.row(i).maxCoeff() - strike(0)) * exp(-risk_free_rate(0) * tau):  pay_off(i, 0) = 0;  
                                    prices_variance_reduction.row(i).minCoeff() - strike(0) > 0? pay_off_variance_reduction(i, 0) = (prices_variance_reduction.row(i).maxCoeff() - strike(0)) * exp(-risk_free_rate(0) * tau):  pay_off_variance_reduction(i, 0) = 0; 
                                    discounted_pay_off(i, 0) = 0.5 * (pay_off(i, 0) + pay_off_variance_reduction(i, 0));
                                    break;
                        
                        }   

                        }else{  

                            for (int j = 0; j < dimensions; j++){ 
                                prices(i, j) - strike(j) > 0? pay_off(i, j) = (prices(i, j) - strike(j)) * exp(-rate(j) * Time):  pay_off(i, j) = 0;  
                                prices_variance_reduction(i, j) - strike(j) > 0? pay_off_variance_reduction(i, j) = (prices_variance_reduction(i, j) - strike(j)) * exp(-rate(j) * Time):  pay_off_variance_reduction(i, j) = 0; 
                                discounted_pay_off(i, j) = 0.5 * (pay_off(i, j) + pay_off_variance_reduction(i, j));
                            }
                        }
                    }
    }else{

        for (int i = 0; i < number_of_iterations; i++){
                    
                    Eigen::MatrixXd A, B; 
                    correlation_matrix.has_value()? std::tie(A, B) = price_generator(*correlation_matrix): std::tie(A, B) = price_generator(std::nullopt);
                    prices.row(i) = A(last, all);

                    if(payoff_calculation){

                            switch(*payoff_calculation){ // * means if(payoff_calculation) then use what is provided inside payoff_calculation
                                
                                case payoff::basket:
                                    
                                    if(weights.has_value())
                                    {(*weights).transpose() * prices.row(i).transpose() - strike(0) > 0? pay_off(i, 0) = ((*weights).transpose() * prices.row(i).transpose() - strike(0)) * exp(-risk_free_rate(0) * tau):  pay_off(i, 0) = 0;   
                                    discounted_pay_off(i, 0) = pay_off(i, 0); }
                                    break; 

                                case payoff::maxim:

                                    prices.row(i).maxCoeff() - strike(0) > 0? pay_off(i, 0) = (prices.row(i).maxCoeff() - strike(0)) * exp(-risk_free_rate(0) * tau):  pay_off(i, 0) = 0;   
                                    discounted_pay_off(i, 0) = pay_off(i, 0);
                                    break; 

                                case payoff::worst_off:

                                    prices.row(i).minCoeff() - strike(0) > 0? pay_off(i, 0) = (prices.row(i).maxCoeff() - strike(0)) * exp(-risk_free_rate(0) * tau):  pay_off(i, 0) = 0;  
                                    discounted_pay_off(i, 0) = pay_off(i, 0);
                                    break;
                        
                            }
                    }else{ 
                    
                        for (int j = 0; j < dimensions; j++){ 
                            prices(i, j) - strike(j) > 0? pay_off(i, j) = (prices(i, j) - strike(j)) * exp(-rate(j) * Time):  pay_off(i, j) = 0;  
                            discounted_pay_off(i, j) = pay_off(i, j);
                        }  
                    }
        }
    }

    return discounted_pay_off;

}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// Class Method 4 ------------------------------------------------------------------------------------------------------------------------------------------
Asset_Option_Price::Option_output  Asset_Option_Price::Monte_Carlo_option_pricer(int number_of_iterations, Eigen::VectorXd risk_free_rate, double tau, bool variance_reduction,
                                                                                std::optional<payoff> payoff_calculation, std::optional<Eigen::VectorXd> weights, Eigen::VectorXd strike,
                                                                                std::optional<Eigen::MatrixXd> correlation_matrix, std::function < std::pair<Eigen::MatrixXd,
                                                                                Eigen::MatrixXd>(std::optional<Eigen::MatrixXd>) > custom_price_generator){
        
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
            if (correlation_matrix.has_value()){
                weights.has_value()? discounted_pay_off = discounted_pay_off_calculator(number_of_iterations, risk_free_rate, tau, true, payoff_calculation,
                                                                   *weights, strike, *correlation_matrix, price_generator):
                                    discounted_pay_off = discounted_pay_off_calculator(number_of_iterations, risk_free_rate, tau, true, payoff_calculation,
                                                                   std::nullopt, strike, *correlation_matrix, price_generator);
            }else{
                weights.has_value()? discounted_pay_off = discounted_pay_off_calculator(number_of_iterations, risk_free_rate, tau, true, payoff_calculation,
                                                                    *weights, strike, std::nullopt, price_generator):
                                    discounted_pay_off_calculator(number_of_iterations, risk_free_rate, tau, true, payoff_calculation,
                                                                    std::nullopt, strike, std::nullopt, price_generator);
            }
        }else{
            if (correlation_matrix.has_value()){
                weights.has_value()? discounted_pay_off = discounted_pay_off_calculator(number_of_iterations, risk_free_rate, tau, false, payoff_calculation,
                                                                    *weights, strike, *correlation_matrix, price_generator):
                                    discounted_pay_off = discounted_pay_off_calculator(number_of_iterations, risk_free_rate, tau, false, payoff_calculation,
                                                                    std::nullopt, strike, *correlation_matrix, price_generator);     
            }else{
                weights.has_value()? discounted_pay_off = discounted_pay_off_calculator(number_of_iterations, risk_free_rate, tau, false, payoff_calculation,
                                                                    *weights, strike, std::nullopt, price_generator):
                                    discounted_pay_off = discounted_pay_off_calculator(number_of_iterations, risk_free_rate, tau, false, payoff_calculation,
                                                                    std::nullopt, strike, std::nullopt, price_generator);
            }
        }
    
    Eigen::VectorXd sample_mean(dimensions); sample_mean.setZero();
    Eigen::VectorXd sample_variance(dimensions); sample_variance.setZero();
    Eigen::VectorXd sample_standard_error(dimensions); sample_standard_error.setZero();
    Eigen::VectorXd ci_lower_bound(dimensions); ci_lower_bound.setZero();
    Eigen::VectorXd ci_upper_bound(dimensions); ci_upper_bound.setZero();
    
    
    // SAMPLE MEAN ------------------------------------------------------------------------------------
    sample_mean = discounted_pay_off.colwise().mean();
    output.sample_mean = sample_mean;  
    
    // SAMPLE VARIANCE --------------------------------------------------------------------------------
    for (int j = 0; j < dimensions; j++){
        for (int i = 0; i < number_of_iterations; i++){
            sample_variance(j) += pow((discounted_pay_off(i, j) - sample_mean(j)), 2);    
        }
        sample_variance(j) = sample_variance(j) / static_cast<double>(number_of_iterations - 1);
    }
    output.sample_variance = sample_variance;
    
    // STANDARD ERROR ------------------------------------------------------------------------------------
    for (int i = 0; i < dimensions; i++){
        sample_standard_error(i) = sqrt(sample_variance(i) / static_cast<double>(number_of_iterations));
    }
    output.sample_standard_error = sample_standard_error;

    // CONFIDENCE INTERVALS -----------------------------------------------------------------------------
    std::stringstream confidence_intervals;

    for(int i = 0; i < dimensions; i++){
        ci_lower_bound(i) = sample_mean(i) - 1.96 * sample_standard_error(i);   
        ci_upper_bound(i) = sample_mean(i) + 1.96 * sample_standard_error(i);
        confidence_intervals << "[" << ci_lower_bound(i) << ": " << ci_upper_bound(i) << "]\n";
    }
    output.confidence_intervals = confidence_intervals.str();
    return output;
}

                                                   