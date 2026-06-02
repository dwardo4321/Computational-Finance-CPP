#include <iostream>
#include <Eigen/Dense>
#include "utilities.hpp"
#include "Asset_Option_Price.hpp"
#include "Delta_Hedging_Engine.hpp"
#include "Risk_Neutral_MultiDim_Delta_Hedging_Engine.hpp"

int main(){
    
    /*
    std::cout << "---------------------------- Monte Carlo Pricer ----------------------------\n";

    Eigen::VectorXd price_today(5); 
    price_today << 1000, 1652, 957, 1091, 1450;
    Eigen::VectorXd rate(5); 
    rate << 0.1, 0.188, 0.05, 0.08, 0.213;
    Eigen::VectorXd strike(5); 
    strike << 1100, 1600, 1000, 1200, 1500;
    Eigen::VectorXd volatility(5); 
    volatility << 0.21, 0.25, 0.15, 0.18, 0.1;
    Eigen::MatrixXd correlation(5, 5);
    correlation << 1.00, 0.35, 0.20, 0.10, 0.25,
                   0.35, 1.00, 0.40, 0.15, 0.30,
                   0.20, 0.40, 1.00, 0.50, 0.45,
                   0.10, 0.15, 0.50, 1.00, 0.55,
                   0.25, 0.30, 0.45, 0.55, 1.00;
    Asset_Option_Price asset_4 = Asset_Option_Price(strike, rate, volatility, price_today, 2, 1000);
    std::cout << asset_4.Monte_Carlo_option_pricer(1000, true, correlation).confidence_intervals << '\n';
    std::cout << asset_4.Monte_Carlo_option_pricer(1000, false, correlation).confidence_intervals << '\n';
    */

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

    int D = 3;   // number of Brownian drivers
    int M = 5;   // number of assets/options
    
    Eigen::VectorXd strike(M);
    strike << 95.0, 100.0, 105.0, 110.0, 115.0;

    Eigen::VectorXd rate(M);
    rate << 0.05, 0.05, 0.05, 0.05, 0.05;

    Eigen::VectorXd price_today(M);
    price_today << 100.0, 102.0, 98.0, 105.0, 110.0;

    Eigen::MatrixXd volatility_realised(M, D);
    volatility_realised <<
        0.20,  0.05,  0.00,
        0.10,  0.25,  0.05,
        0.00,  0.15,  0.30,
        0.18, -0.05,  0.12,
        0.07,  0.10,  0.22;

    Eigen::MatrixXd volatility_implied(M, D);
    volatility_implied <<
        0.22,  0.06,  0.01,
        0.11,  0.27,  0.06,
        0.01,  0.16,  0.32,
        0.19, -0.04,  0.13,
        0.08,  0.11,  0.24;

    double Time = 1.0;              
    int discretisation = 1000;       

    Multidimensional_Risk_Neutral_Engine test_1 = Multidimensional_Risk_Neutral_Engine(strike, rate, price_today, volatility_realised, volatility_implied, Time, discretisation);
    std::cout << test_1.Risk_Neutral_MultiDim_DHE(true); 
}                                                                                                                                        