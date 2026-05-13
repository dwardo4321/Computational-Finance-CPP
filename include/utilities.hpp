#include <iostream>
#include <Eigen/Dense>
#include <optional>

class utility{

    public:

        Eigen::MatrixXd Brownian_path_generator(int discretisation_brownian_motion, double Time, std::optional<Eigen::MatrixXd> correlation_matrix);
        
        std::pair <Eigen::MatrixXd, Eigen::MatrixXd> GBM_price_path_generator(Eigen::VectorXd strike,     
                                                                              Eigen::VectorXd rate,         // fixed rate
                                                                              Eigen::VectorXd volatility,   // fixed volatility
                                                                              Eigen::VectorXd price_today,
                                                                              double Time,                  // time duration
                                                                              int dimensions,               // number of assets
                                                                              int discretisation_brownian_motion, // number of steps 
                                                                              std::optional<Eigen::MatrixXd> correlation_matrix);
}
