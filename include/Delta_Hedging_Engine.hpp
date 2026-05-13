#include <iostream>
#include <string>
#include <Vector>
#include <Eigen/Dense>
#include <optional>

class Delta_Hedging_Engine_Time_Dep_Vol{
    
    private:

        double strike;     
        double rate;         // fixed rate
        double volatility_realised;   // volatility
        double price_today;
        double Time;                  // time duration
        int dimensions;               // number of assets
        int discretisation; // number of steps 

        std::pair <Eigen::MatrixXd, Eigen::MatrixXd> GBM_price_path_generator(std::optional<Eigen::MatrixXd> correlation_matrix);

        Eigen::MatrixXd Brownian_path_generator(std::optional<Eigen::MatrixXd> correlation_matrix);

    public:

        Eigen::MatrixXd DHE_Time_Dependent_Volatility();

};



