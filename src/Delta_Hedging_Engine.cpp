#include "Delta_Hedging_Engine_Time_Dep_Vol.hpp"

#include <iostream>
#include <Eigen/Dense>
#include <random>
#include <cmath>
#include <Vector>
#include <optional>

// Private Methods ---------------------------------------------------------


// Method 1 ----------------------------------------------------------------
Eigen::MatrixXd Delta_Hedging_Engine_Time_Dep_Vol::DHE_Time_Dependent_Volatility(bool call){

    Eigen::MatrixXd output(discretisation, 10);
    output.row(0).setZero();

    double d_1;
    double d_2;
    double stock_price;

    for (int i; i < discretisation; i++){

        GBM_price_path_generator();
        
        d_1 = log(stock_price / strike) + (rate + 0.5 * std::pow(volatility_realised, 2)) * (Time - i); 
        
        d_2 = d_1 - volatility_realised * sqrt(Time - i)

    }
 
    return output;

}
