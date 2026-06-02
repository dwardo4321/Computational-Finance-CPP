#include "Delta_Hedging_Engine.hpp"
#include "utilities.hpp"

#include <iostream>
#include <Eigen/Dense>
#include <boost/math/distributions.hpp>
#include <random>
#include <cmath>
#include <vector>
#include <optional>

namespace{
    std::random_device rd;
    std::mt19937_64 gen(rd());
}

// Constructor ---------------------------------------------------------
Delta_Hedging_Engine::Delta_Hedging_Engine(double strike_constr, double rate_constr, double volatility_realised_constr,   
                                           double volatility_implied_constr, double price_today_constr, double Time_constr, int discretisation_constr):
                                           strike(strike_constr), rate(rate_constr), volatility_realised(volatility_realised_constr),
                                           volatility_implied(volatility_implied_constr), price_today(price_today_constr), Time(Time_constr), discretisation(discretisation_constr)
                                           {}                                    

// Method 1 ----------------------------------------------------------------
Eigen::MatrixXd Delta_Hedging_Engine::DHE_Time_Dependent_Volatility(bool call, double long_run_implied_vol, double mean_reversion_rate){

    Eigen::MatrixXd output(discretisation, 14);
    output.row(0).setZero();

    double d_1;
    double d_2;

    Eigen::VectorXd stock_price(discretisation);
    stock_price(0) = price_today;

    Eigen::VectorXd dynamic_volatility_implied(discretisation);
    dynamic_volatility_implied(0) = volatility_implied;

    Eigen::VectorXd dynamic_volatility_realised(discretisation);
    dynamic_volatility_realised.segment(0, 51).setConstant(volatility_realised);

    Eigen::VectorXd delta(discretisation); 
    delta = Eigen::VectorXd::Zero(discretisation);
    Eigen::VectorXd gamma(discretisation); 
    gamma = Eigen::VectorXd::Zero(discretisation);
    Eigen::VectorXd theta(discretisation);
    theta = Eigen::VectorXd::Zero(discretisation);

    Eigen::VectorXd option(discretisation); 
    option = Eigen::VectorXd::Zero(discretisation);
    Eigen::VectorXd bank_balance(discretisation); 
    bank_balance = Eigen::VectorXd::Zero(discretisation);
    Eigen::VectorXd portfolio(discretisation);
    portfolio = Eigen::VectorXd::Zero(discretisation);

    Eigen::VectorXd local_PandL(discretisation);
    local_PandL = Eigen::VectorXd::Zero(discretisation);
    Eigen::VectorXd gamma_gain(discretisation);
    gamma_gain = Eigen::VectorXd::Zero(discretisation);
    Eigen::VectorXd theta_decay(discretisation);
    theta_decay = Eigen::VectorXd::Zero(discretisation);

    Eigen::VectorXd hedging_error(discretisation);
    hedging_error = Eigen::VectorXd::Zero(discretisation);

    Eigen::VectorXd time_steps(discretisation); 
    time_steps = Eigen::VectorXd::Zero(discretisation);
      
    double dt = static_cast<double>(Time) / (discretisation - 1);
    std::normal_distribution<double> standard_norm(0.0, 1.0);

    // Initialisation --------------------------------------------------------------------
    d_1 = (log(stock_price(0) / strike) + (rate + 0.5 * std::pow(dynamic_volatility_implied(0), 2)) * Time) / (dynamic_volatility_implied(0) * std::sqrt(Time)); 
    d_2 = d_1 - dynamic_volatility_implied(0) * sqrt(Time);

    gamma(0) = stats::normal_pdf(d_1) / (stock_price(0) * dynamic_volatility_implied(0) * std::sqrt(Time));
    
    if (call){

        delta(0) = stats::normal_cdf(d_1);
        theta(0) = -(stock_price(0) * stats::normal_pdf(d_1) * dynamic_volatility_implied(0)) / (2 * std::sqrt(Time)) - rate * strike * exp(-rate * Time) * stats::normal_cdf(d_2);
        option(0) = stock_price(0) * stats::normal_cdf(d_1) - strike * exp(-rate * Time) * stats::normal_cdf(d_2);
    
    }else{

        delta(0) = -stats::normal_cdf(-d_1);
        theta(0) = -(stock_price(0) * stats::normal_pdf(d_1) * dynamic_volatility_implied(0)) / (2 * std::sqrt(Time)) + rate * strike * exp(-rate * Time) * stats::normal_cdf(-d_2);
        option(0) = strike * exp(-rate * Time) * stats::normal_cdf(-d_2) - stock_price(0) * stats::normal_cdf(-d_1);
    }

    bank_balance(0) = option(0) - delta(0) * stock_price(0);
    portfolio(0) = bank_balance(0) + delta(0) * stock_price(0);

    // Engine ----------------------------------------------------------------------------
    for (int i = 1; i < discretisation ; i++){

        time_steps[i] = i * dt;
        double tau = Time - i * dt;
        
        // Asset price and realised volatility -------------------------------------------
        if (i < 51){

            stock_price[i] = stock_price[i-1] * std::exp((rate - 0.5 * pow(dynamic_volatility_realised(i), 2)) * dt + dynamic_volatility_realised(i) * (std::sqrt(dt) * standard_norm(gen)));
        
        }else{

            Eigen::ArrayXd log_returns = (stock_price.segment(i-50, 50).array() / stock_price.segment(i-51, 50).array()).log();

            double var = (log_returns - log_returns.mean()).square().sum() / 49;

            dynamic_volatility_realised[i] = std::sqrt(var / dt); //Var(log_returns​) ≈ vol^2 * Δt

            stock_price[i] = stock_price[i-1] * std::exp((rate - 0.5 * pow(dynamic_volatility_realised(i), 2)) * dt + dynamic_volatility_realised[i] * (std::sqrt(dt) * standard_norm(gen)));
        }

        dynamic_volatility_implied[i] = long_run_implied_vol + (volatility_implied - long_run_implied_vol) * exp(-mean_reversion_rate * i * dt);

        // Terminal ----------------------------------------------------------------
        if (i == (discretisation - 1)){

            if (call){option(i) = std::max(stock_price(i) - strike, 0.0);}

            else{option(i) = std::max(strike - stock_price(i), 0.0);}

            gamma(i) = 0;
            theta(i) = 0;
            delta(i) = delta(i - 1);

            bank_balance(i) = bank_balance(i - 1) * std::exp(rate * dt);

            portfolio(i) = bank_balance(i) + delta(i) * stock_price(i);

            hedging_error(i) = option(i) - portfolio(i);

            double delta_price = stock_price(i) - stock_price(i - 1);

            gamma_gain(i) = 0.5 * gamma(i - 1) * std::pow(delta_price, 2);

            theta_decay(i) = theta(i - 1) * dt;

            local_PandL(i) = 0.5 * gamma(i - 1) * std::pow(stock_price(i - 1), 2) * (std::pow(dynamic_volatility_realised(i - 1), 2) - std::pow(dynamic_volatility_implied(i - 1), 2)) * dt;

            continue;
        }

        // Option and Greeks  ------------------------------------------- 
        d_1 = (log(stock_price(i) / strike) + (rate + 0.5 * std::pow(dynamic_volatility_implied[i], 2)) * tau) / (dynamic_volatility_implied[i] * std::sqrt(tau)); 
        
        d_2 = d_1 - dynamic_volatility_implied[i] * sqrt(tau);

        gamma[i] = stats::normal_pdf(d_1) / (stock_price[i] * dynamic_volatility_implied[i] * std::sqrt(tau));

        if (call){

            delta[i] = stats::normal_cdf(d_1);

            theta[i] = -(stock_price[i] * stats::normal_pdf(d_1) * dynamic_volatility_implied[i]) / (2 * std::sqrt(tau)) - rate * strike * exp(-rate * tau) * stats::normal_cdf(d_2);

            option[i] = stock_price[i] * stats::normal_cdf(d_1) - strike * exp(-rate * tau) * stats::normal_cdf(d_2);    

        }else{

            delta[i] = -stats::normal_cdf(-d_1);

            theta[i] = -(stock_price[i] * stats::normal_pdf(d_1) * dynamic_volatility_implied[i]) / (2 * std::sqrt(tau)) + rate * strike * exp(-rate * tau) * stats::normal_cdf(-d_2);

            option[i] = strike * exp(-rate * tau) * stats::normal_cdf(-d_2) - stock_price[i] * stats::normal_cdf(-d_1);
        }

        // Portfolio -------------------------------------------
        bank_balance[i] = bank_balance[i-1] * exp(rate * dt) - (delta[i] - delta[i - 1]) * stock_price[i];
        
        portfolio[i] = bank_balance[i] + delta[i] * stock_price[i];       
        
        // Local P&L -------------------------------------------
        local_PandL[i] = 0.5 * gamma[i - 1] * pow(stock_price[i - 1], 2) * (pow(dynamic_volatility_realised[i - 1], 2) - pow(dynamic_volatility_implied[i - 1], 2)) * dt;
        
        double delta_price = stock_price[i] - stock_price[i - 1];
        gamma_gain[i] = 0.5 * gamma[i - 1] * pow(delta_price, 2);

        double price_vol = dynamic_volatility_implied[i] * stock_price[i];
        theta_decay[i] = theta[i - 1] * dt;

        // Hedging error -------------------------------------------
        hedging_error[i] = option[i] - portfolio[i];

        }
          
    // Final Table -------------------------------------------------------
    output.col(0) = time_steps;
    
    output.col(1) = stock_price;
    output.col(2) = bank_balance;
    output.col(3) = option;
    output.col(4) = portfolio;
    output.col(5) = hedging_error;

    output.col(6) = dynamic_volatility_implied;
    output.col(7) = dynamic_volatility_realised;

    output.col(8) = delta;
    output.col(9) = theta;
    output.col(10) = gamma;

    output.col(11) = gamma_gain;
    output.col(12) = theta_decay;
    output.col(13) = local_PandL;

    return output;

}





