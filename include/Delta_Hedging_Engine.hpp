#pragma once

#include <iostream>
#include <Eigen/Dense>
#include <optional>

class Delta_Hedging_Engine{
    
    private:

        double strike;     
        double rate;         // fixed rate
        double volatility_realised;   // asset volatility
        double volatility_implied;   // option volatility
        double price_today;
        double Time;                  // time duration
        int discretisation; // number of steps 

    public:

        // Constructor
        Delta_Hedging_Engine(double strike_constr, double rate_constr, double volatility_realised_constr,   
                            double volatility_implied_constr, double price_today_constr, double Time_constr, int discretisation_constr);

        // Method 1
        Eigen::MatrixXd DHE_Time_Dependent_Volatility(bool call, double long_run_implied_vol = 0.25, double mean_reversion_rate = 0.75);

};



