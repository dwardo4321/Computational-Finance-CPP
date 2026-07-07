#include <iostream>
#include <fstream> //csv ouput
#include <Eigen/Dense>
#include "utilities.hpp"
#include "Asset_Option_Price.hpp"
#include "Delta_Hedging_Engine.hpp"
#include "Risk_Neutral_MultiDim_Delta_Hedging_Engine.hpp"

int main(){
    
    
    std::cout << "---------------------------- Monte Carlo Pricer ----------------------------\n";

    Eigen::VectorXd price_today(5); 
    price_today << 1000, 1652, 957, 1091, 1450;
    //Eigen::VectorXd risk_free_rate(5);
    //risk_free_rate << 0.04, 0.04, 0.04, 0.04, 0.04;
    double risk_free_rate_1 = 0.04;

    Eigen::VectorXd rate(5); 
    rate << 0.1, 0.188, 0.05, 0.08, 0.213;
    //Eigen::VectorXd strike(5); 
    //strike << 1100, 1600, 1000, 1200, 1500;
    double strike = 1000; 

    Eigen::VectorXd volatility(5); 
    volatility << 0.21, 0.25, 0.15, 0.18, 0.1; 
    Eigen::VectorXd weights(5);
    weights << 0.2, 0.2, 0.2, 0.2, 0.2;
    Eigen::MatrixXd correlation(5, 5);
    correlation << 1.00, 0.35, 0.20, 0.10, 0.25,
                   0.35, 1.00, 0.40, 0.15, 0.30,
                   0.20, 0.40, 1.00, 0.50, 0.45,
                   0.10, 0.15, 0.50, 1.00, 0.55,
                   0.25, 0.30, 0.45, 0.55, 1.00;

    double Time = 2.00;
    int discretisation = 1000;

    /* double price_today = 958; 
    double risk_free_rate = 0.04;

    double rate = 0.05; 
    double strike = 1000; 

    double volatility = 0.15; */ 

    Asset_Option_Price asset_4 = Asset_Option_Price(strike, rate, risk_free_rate_1, volatility, price_today, Time, discretisation);
    std::cout << asset_4.Monte_Carlo_option_pricer(1000, risk_free_rate_1, 10, false, Asset_Option_Price::payoff::maxim, std::nullopt, strike, std::nullopt, nullptr).sample_mean << '\n';
    //std::cout << asset_4.discounted_pay_off_calculator(1000, risk_free_rate, 999, true, Asset_Option_Price::payoff::maxim, weights, strike, correlation, nullptr) << '\n';
   

    // -------------------------------------------------------------------------

    /*
    double strike = 1000;     
    double rate = 0.225;         // fixed rate
    double volatility_realised = 0.35;   // asset volatility
    double volatility_implied = 0.398;   // option volatility
    double price_today = 925;
    double Time = 5;                  // time duration
    int discretisation = 1000; // number of steps
    
    Delta_Hedging_Engine asset_1 = Delta_Hedging_Engine(strike, rate, volatility_realised, volatility_implied, price_today, Time, discretisation);
    std::cout << "----------------------------------------------------------------------------Delta Hedging Engine--------------------------------------------------------------------------------\n";
    std::cout << asset_1.DHE_Time_Dependent_Volatility(true) << '\n';
    std::cout << "--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
    */

    /* int D = 4;   // number of Brownian drivers
    int M = 5;   // number of assets/options

    Eigen::VectorXd strike(M);
    strike << 90.0, 100.0, 110.0, 120.0, 130.0;

    Eigen::VectorXd rate(M);
    rate << 0.06, 0.055, 0.05, 0.052, 0.058;

    Eigen::VectorXd risk_free_rate(M);
    risk_free_rate << 0.04, 0.04, 0.04, 0.04, 0.04;

    Eigen::VectorXd price_today(M);
    price_today << 92.0, 104.0, 111.0, 118.0, 137.0;

    Eigen::MatrixXd volatility_realised(M, D);
    volatility_realised <<
        0.22,  0.10, -0.05,  0.18,
        0.15,  0.28,  0.12, -0.04,
        -0.08,  0.19,  0.31,  0.09,
        0.27, -0.06,  0.14,  0.25,
        0.11,  0.23, -0.09,  0.34;

    Eigen::MatrixXd volatility_implied(M, D);
    volatility_implied <<
        0.25,  0.12, -0.03,  0.20,
        0.17,  0.30,  0.15, -0.02,
        -0.06,  0.21,  0.34,  0.11,
        0.29, -0.04,  0.17,  0.27,
        0.13,  0.25, -0.07,  0.37;

    double Time = 1.0;              
    int discretisation = 1000;       

    Multidimensional_Risk_Neutral_Engine test_1 = Multidimensional_Risk_Neutral_Engine(strike, rate, risk_free_rate, price_today, volatility_realised, volatility_implied, Time, discretisation);
    Eigen::MatrixXd output = test_1.Risk_Neutral_MultiDim_DHE(true);

    std::ofstream file("C:/Users/Tapson/Downloads/output.csv");
    Eigen::IOFormat csv_format(Eigen::StreamPrecision, Eigen::DontAlignCols, ",", "\n");

    file << output.format(csv_format);
    file.close();

    std::cout << output; */
}                                                                                                                                        