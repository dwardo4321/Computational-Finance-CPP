#include "utilities.hpp"

#include <iostream>
#include <Eigen/Dense>
#include <optional>


// Method 1 -----------------------------------------------------------------
Eigen::MatrixXd  utilities::Brownian_path_generator(int discretisation_brownian_motion, double Time, std::optional<Eigen::MatrixXd> correlation_matrix){
    
    Eigen::MatrixXd paths(discretisation_brownian_motion, dimensions);
    paths.row(0).setZero();

    double delta_time = static_cast<double>(Time) / (discretisation_brownian_motion - 1);
    
    // Generate Z ~ N(0, I_(nxn))   
    std::normal_distribution<double> standard_norm(0, 1);
    Eigen::MatrixXd standard_normal_rv(discretisation_brownian_motion, dimensions);
    for (int i = 0; i < dimensions; i++)
    {
        for(int j = 0; j < discretisation_brownian_motion; j++)
        {
            standard_normal_rv(j, i) = standard_norm(gen1);
        }                 
    }

    if(correlation_matrix.has_value()){

        // Cholesky Decomposition
        Eigen::LLT<Eigen::MatrixXd> llt(correlation_matrix.value());
        Eigen::MatrixXd lower_triangular = llt.matrixL();
        
        // dW(t), and BM (Brownian Motion) 
        Eigen::VectorXd delta_BM;
    
        for(int i = 1; i < discretisation_brownian_motion; i++)
        {
            delta_BM = sqrt(delta_time) * (lower_triangular * standard_normal_rv.row(i).transpose());
            paths.row(i) = paths.row(i - 1) + delta_BM.transpose();
        }

    } 
    else
    {
        for(int i = 1; i < discretisation_brownian_motion; i++)
        {
            paths.row(i) = paths.row(i - 1) + standard_normal_rv.row(i) * sqrt(delta_time);
        }
    }

    return paths; 
};

// Method 2 -----------------------------------------------------------------
std::pair <Eigen::MatrixXd, Eigen::MatrixXd> utilities::GBM_price_path_generator(Eigen::VectorXd strike,     
                                                                                 Eigen::VectorXd rate,         // fixed rate
                                                                                 Eigen::VectorXd volatility,   // fixed volatility
                                                                                 Eigen::VectorXd price_today,
                                                                                 double Time,                  // time duration
                                                                                 int dimensions,               // number of assets
                                                                                 int discretisation_brownian_motion, // number of steps 
                                                                                 std::optional<Eigen::MatrixXd> correlation_matrix){ 
    
    // Pass-by-Reference (additional matrix prices_paths_variance_reduction)
    Eigen::MatrixXd brownian_paths(discretisation_brownian_motion, dimensions);
    brownian_paths = Brownian_path_generator(correlation_matrix);
    
    Eigen::MatrixXd prices_paths(discretisation_brownian_motion, dimensions);
    Eigen::MatrixXd prices_paths_variance_reduction(discretisation_brownian_motion, dimensions);
    
    prices_paths.row(0) = price_today.transpose();
    prices_paths_variance_reduction.row(0) = price_today.transpose();

    //Eigen::VectorXd time_steps = Eigen::VectorXd::LinSpaced(discretisation_brownian_motion, 0, Time);

    double delta_time = static_cast<double>(Time) / (discretisation_brownian_motion - 1);

    for (int i = 0; i < dimensions; i++){  

        for (int j = 1; j < discretisation_brownian_motion; j++){ 

            double delta_BM = brownian_paths(j, i) - brownian_paths(j - 1, i);

            double X = volatility(i) * delta_BM + (rate(i) - 0.5*pow(volatility(i), 2)) * delta_time;
            double X_vd = volatility(i) * -delta_BM + (rate(i) - 0.5*pow(volatility(i), 2)) * delta_time;

            prices_paths(j, i) = prices_paths(j - 1, i) * exp(X);   
            prices_paths_variance_reduction(j, i) = prices_paths_variance_reduction(j - 1, i) * exp(X_vd);               
        }
    }
    return {prices_paths, prices_paths_variance_reduction};
}