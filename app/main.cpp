#include <iostream>
#include <fstream> //csv ouput
#include <Eigen/Dense>
#include "utilities.hpp"
#include "Asset_Option_Price.hpp"
#include "Delta_Hedging_Engine.hpp"
#include "Risk_Neutral_MultiDim_Delta_Hedging_Engine.hpp"

int main(){
    
    
    //std::cout << "---------------------------- Monte Carlo Pricer ----------------------------\n";

    /* double strike = 1000;
    double rate = 0.08;
    double risk_free_rate = 0.05;
    double volatility = 0.20;
    double price_today = 1230;
    double Time = 1;
    int discretisation = 252; */

    /* double strike = 1000;
    Eigen::VectorXd rate(3); rate << 0.08, 0.07, 0.09;
    double risk_free_rate = 0.05;
    Eigen::MatrixXd volatility(3, 1); volatility << 0.20, 0.25, 0.30;
    Eigen::VectorXd price_today(3); price_today << 1000, 950, 1050;
    double Time = 1;
    int discretisation = 252; */

    /* double strike = 900;
    Eigen::VectorXd rate(4); rate << 0.08, 0.075, 0.09, 0.065;
    double risk_free_rate = 0.05;
    Eigen::VectorXd volatility(4); volatility << 0.20, 0.25, 0.30, 0.18;
    Eigen::VectorXd price_today(4); price_today << 1000, 950, 1050, 980;
    double Time = 1;
    int discretisation = 252; */
    
    /* Minimum_Asset payoff_object(strike);

    Asset_Option_Price asset_4 = Asset_Option_Price(strike, rate, risk_free_rate, volatility, price_today, Time, discretisation);
    std::cout << asset_4.Monte_Carlo_option_pricer(1000, risk_free_rate, 30, false, std::nullopt, payoff_object, nullptr).sample_mean << '\n';
    //std::cout << asset_4.discounted_pay_off_calculator(1000, risk_free_rate, 50, true, correlation, payoff_object, nullptr) << '\n';
    std::cout << asset_4.discounted_pay_off_calculator(1000, risk_free_rate, 50, true, std::nullopt, payoff_object, nullptr) << '\n';
    //std::cout << asset_4.GBM_price_path(correlation)<< '\n'; */

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

    int D = 4;   // number of Brownian drivers
    int M = 5;   // number of assets/options

    double strike = 90.0;

    Eigen::VectorXd rate(M);
    rate << 0.06, 0.055, 0.05, 0.052, 0.058;

    double risk_free_rate = 0.09;

    Eigen::VectorXd price_today(M);
    price_today << 92.0, 104.0, 111.0, 118.0, 137.0;

    Eigen::VectorXd initial_price(M);
    initial_price << 102.05, 114.98, 95.0, 101.90, 125.85;

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
    int iterations = 1000;     

    Multidimensional_Risk_Neutral_Engine test_1 = Multidimensional_Risk_Neutral_Engine(strike, rate, risk_free_rate, price_today, volatility_realised, volatility_implied, Time, discretisation);
    Eigen::MatrixXd output = test_1.Risk_Neutral_MultiDim_DHE(true);

    Eigen::VectorXd weights(5);
    weights << 0.3, 0.3, 0.2, 0.1, 0.05;

    Basket_Assets payoff_object(strike, weights);  

    std::vector<Eigen::MatrixXd> std_normal_rv_bank(iterations);
    
    std::random_device rd;
    std::mt19937_64 generator(rd());

    for(int i = 0; i < iterations; i++){std_normal_rv_bank[i] = utility::Normal_RV_generator(discretisation, price_today.size(), generator);}

    Multidimensional_Risk_Neutral_Engine::quad output123 = test_1.Greeks_and_Option(iterations, 990, false, initial_price, std::nullopt, std_normal_rv_bank, payoff_object, nullptr);

    std::cout << output123.Option;

    /* std::ofstream file("C:/Users/Tapson/Downloads/output.csv");
    Eigen::IOFormat csv_format(Eigen::StreamPrecision, Eigen::DontAlignCols, ",", "\n");

    file << output.format(csv_format);
    file.close();

    std::cout << output; */
}                                                                                                                                        