#pragma once

#include <iostream>
#include <Eigen/Dense>
#include <optional>
#include <random>

namespace stats{
    double normal_cdf(double x);
    double normal_pdf(double x);
}
namespace utility{   

    Eigen::MatrixXd Normal_RV_generator(int discretisation_brownian_motion, int dimensions, std::mt19937_64& generator);
    
    Eigen::MatrixXd Brownian_path_generator(int discretisation_brownian_motion, int dimensions, double Time, const Eigen::MatrixXd& standard_normal_rv, std::optional<Eigen::MatrixXd> correlation_matrix);

    std::pair <Eigen::MatrixXd, Eigen::MatrixXd> GBM_price_path_generator(Eigen::VectorXd strike,     
                                                                                 Eigen::VectorXd rate,         // fixed rate
                                                                                 Eigen::VectorXd volatility,   // fixed volatility
                                                                                 Eigen::VectorXd price_today,
                                                                                 double Time,                  // time duration
                                                                                 int dimensions,               // number of assets
                                                                                 int discretisation_brownian_motion, // number of steps
                                                                                 const Eigen::MatrixXd& standard_normal_rv, 
                                                                                 std::optional<Eigen::MatrixXd> correlation_matrix);
};
