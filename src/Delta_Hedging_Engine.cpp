#include "Delta_Hedging_Engine.hpp"
#include "utilities.hpp"

#include <iostream>
#include <Eigen/Dense>
#include <boost/math/distributions.hpp>
#include <random>
#include <cmath>
#include <Vector>
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

    Eigen::MatrixXd output(discretisation, 13);
    output.row(0).setZero();

    double d_1;
    double d_2;

    Eigen::VectorXd stock_price(discretisation);
    stock_price(0) = price_today;

    Eigen::VectorXd dynamic_volatility_implied(discretisation);
    dynamic_volatility_implied(0) = volatility_implied;

    Eigen::VectorXd dynamic_volatility_realised(discretisation);
    dynamic_volatility_realised.segment(0, 6).setConstant(volatility_realised);

    Eigen::VectorXd delta(discretisation); 
    Eigen::VectorXd gamma(discretisation); 
    Eigen::VectorXd theta(discretisation); 

    Eigen::VectorXd option(discretisation); 
    Eigen::VectorXd bank_balance(discretisation); 
    Eigen::VectorXd portfolio(discretisation);

    Eigen::VectorXd local_PandL(discretisation);
    Eigen::VectorXd gamma_gain(discretisation);
    Eigen::VectorXd theta_decay(discretisation);

    Eigen::VectorXd hedging_error(discretisation);
      
    double dt = static_cast<double>(Time) / (discretisation - 1);
    std::normal_distribution<double> standard_norm(0.0, 1.0);

    for (int i = 0; i < discretisation - 1; i++){
        
        // Asset price and realised volatility -------------------------------------------
        if (i < 6){

            stock_price[i+1] = price_today * std::exp(rate * dt + dynamic_volatility_realised(i) * (std::sqrt(dt) * standard_norm(gen)));
        
        }else{

            Eigen::ArrayXd log_returns = (stock_price.segment(i-5, 5).array() / stock_price.segment(i-6, 5).array()).log();

            double var = 0.2 * (log_returns - log_returns.mean()).square().sum();

            dynamic_volatility_realised[i] = std::sqrt(var) / std::sqrt(dt); //Var(log_returns​) ≈ vol^2 * Δt

            stock_price(i+1) = price_today * std::exp(rate * dt + dynamic_volatility_realised[i] * (std::sqrt(dt) * standard_norm(gen)));
        }

        // Option and Implied volatility -------------------------------------------
        dynamic_volatility_implied[i] = long_run_implied_vol + (volatility_implied - long_run_implied_vol) * exp(-mean_reversion_rate * i);
        
        d_1 = log(stock_price(i) / strike) + (rate + 0.5 * std::pow(dynamic_volatility_implied[i], 2)) * (Time - i); 
        
        d_2 = d_1 - dynamic_volatility_implied[i] * sqrt(Time - i);

        gamma[i] = stats::normal_pdf(d_1) / stock_price[i-1] * dynamic_volatility_implied[i] * std::sqrt(Time - i);

        if (call){

            delta[i] = stats::normal_cdf(d_1);

            theta[i] = -(stock_price[i] * stats::normal_cdf(d_1) * dynamic_volatility_implied[i]) / (2 * std::sqrt(Time - 1)) - rate * strike * exp(-rate * (Time-i)) * stats::normal_pdf(d_2);

            option[i] = stock_price[i] * stats::normal_cdf(d_1) - strike * exp(-rate * (Time - i)) * stats::normal_cdf(d_2);

        }else{

            delta[i] = -stats::normal_cdf(-d_1);

            theta[i] = -(stock_price[i] * stats::normal_cdf(d_1) * dynamic_volatility_implied[i]) / (2 * std::sqrt(Time - 1)) + rate * strike * exp(-rate * (Time-i)) * stats::normal_pdf(-d_2);

            option[i] = strike * exp(-rate * (Time - i)) * stats::normal_cdf(-d_2) - stock_price[i] * stats::normal_cdf(-d_1);
        }

        // Portfolio -------------------------------------------
        bank_balance(0) = option(0) - delta(0) * stock_price(0);
        bank_balance[i + 1] = bank_balance[i] * exp(rate * dt) - (delta[i + 1] - delta[i]) * stock_price[i + 1];

        portfolio(0) = bank_balance(0) + delta(0) * stock_price(0);
        portfolio[i + 1] = bank_balance[i + 1] + delta[i + 1] * stock_price[i + 1];

        // Local P&L -------------------------------------------
        local_PandL[i] = 0.5 * gamma[i] * pow(stock_price[i], 2) * (pow(dynamic_volatility_realised[i], 2) - pow(dynamic_volatility_implied[i], 2)) * dt;
        
        double delta_price = dynamic_volatility_realised[i] * stock_price[i] * std::sqrt(dt) * standard_norm(gen);
        gamma_gain[i] = 0.5 * gamma[i] * pow(delta_price, 2);

        double price_vol = dynamic_volatility_implied[i] * stock_price[i];
        theta_decay[i] = -0.5 * pow(price_vol, 2) * gamma[i];

        // Hedging error -------------------------------------------
        hedging_error[i] = option[i] - portfolio[i];

        }

    output.col(0) = stock_price;
    output.col(1) = bank_balance;
    output.col(2) = option;
    output.col(3) = portfolio;
    output.col(4) = hedging_error;

    output.col(5) = dynamic_volatility_implied;
    output.col(6) = dynamic_volatility_realised;

    output.col(7) = delta;
    output.col(8) = theta;
    output.col(9) = gamma;

    output.col(10) = gamma_gain;
    output.col(11) = theta_decay;
    output.col(12) = local_PandL;

    return output;

}



