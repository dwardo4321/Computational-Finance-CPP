#include <iostream>
#include <Eigen/Dense>
#include "Delta_Hedging_Engine.hpp"

int main(){

    std::cout << "---------------------------- Delta Hedging Engine ----------------------------\n";

    double strike = 1000;     
    double rate = 0.12;         // fixed rate
    double volatility_realised = 0.25;   // asset volatility
    double volatility_implied = 0.23;   // option volatility
    double price_today = 925;
    double Time = 3;                  // time duration
    int discretisation = 1000; // number of steps
    
    Delta_Hedging_Engine asset_1 = Delta_Hedging_Engine(strike, rate, volatility_realised, volatility_implied, price_today, Time, discretisation);
    std::cout << asset_1.DHE_Time_Dependent_Volatility(true) << '\n';
    std::cout << asset_1.DHE_Time_Dependent_Volatility(false) << '\n';

    std::cout << "------------------------------------------------------------------------------\n";

    return 0;
}                                                                                                                                        