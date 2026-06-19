#include "Risk_Neutral_MultiDim_Delta_Hedging_Engine.hpp"
#include "utilities.hpp"

#include <Eigen/Dense>
#include <Eigen/Core>
#include <boost/math/distributions.hpp>
#include <iostream>
#include <cmath>
#include <random>

namespace{
    std::random_device rd;
    std::mt19937_64 gen(rd());
}

using namespace Eigen::placeholders;

// Constructor ---------------------------------------------------------
Multidimensional_Risk_Neutral_Engine::Multidimensional_Risk_Neutral_Engine(Eigen::VectorXd strike_const, Eigen::VectorXd rate_const, Eigen::VectorXd risk_free_rate_const, Eigen::VectorXd price_today_const,
                                                                            Eigen::MatrixXd volatility_realised_const, Eigen::MatrixXd volatility_implied_const, 
                                                                            double Time_const, int discretisation_const):

                                                                            strike(strike_const),     
                                                                            rate(rate_const),         // fixed rates
                                                                            risk_free_rate(risk_free_rate_const),
                                                                            price_today(price_today_const),

                                                                            volatility_realised(volatility_realised_const),   // assets volatility
                                                                            volatility_implied(volatility_implied_const),  // option volatilities
                                                                            
                                                                            Time(Time_const),                  // time duration
                                                                            discretisation(discretisation_const)
                                                                            {} // number of steps 


// Private Method 1 ---------------------------------------------------------
std::pair <Eigen::MatrixXd, Eigen::MatrixXd> Multidimensional_Risk_Neutral_Engine::Multidimensional_GBM(std::optional<Eigen::MatrixXd> correlation_matrix){

    int M = volatility_realised.rows();
    int D = volatility_realised.cols();
    int dimensions = volatility_realised.cols();

    Eigen::MatrixXd brownian_mot = utility::Brownian_path_generator(discretisation, dimensions, Time, correlation_matrix);

    Eigen::VectorXd risk_premium = rate - risk_free_rate;

    Eigen::VectorXd mqt_price_of_risk = volatility_realised.completeOrthogonalDecomposition().solve(risk_premium);
    
    brownian_mot.rowwise() += mqt_price_of_risk.transpose();

    /*
    for(int i = 0; i < D; i++){
        for(int t = 0; t < discretisation; t++){
            brownian_mot(t, i) += mqt_price_of_risk(i);
        }
    }
    */
    
    Eigen::MatrixXd price(discretisation, price_today.size());
    Eigen::MatrixXd price_variance_reduction(discretisation, price_today.size());

    price.row(0) = price_today.transpose();
    price_variance_reduction.row(0) = price_today.transpose();

    Eigen::MatrixXd change_in_price = Eigen::MatrixXd::Zero(discretisation, price_today.size());
    Eigen::MatrixXd change_in_price_vd = Eigen::MatrixXd::Zero(discretisation, price_today.size());

    Eigen::MatrixXd vol = Eigen::MatrixXd::Zero(discretisation, M);
    Eigen::MatrixXd vol_vd = Eigen::MatrixXd::Zero(discretisation, M);

    double dt = Time / static_cast<double>(discretisation - 1);

    for (int t = 1; t < discretisation; t++){

        for(int j = 0; j < M; j++){

            for (int i = 0; i < D; i++){
                
                vol(t, j) += volatility_realised(j, i) * (brownian_mot(t, i) - brownian_mot(t-1, i));
                vol_vd(t, j) += volatility_realised(j, i) * -(brownian_mot(t, i) - brownian_mot(t-1, i));
            } 
        }

        change_in_price.row(t) = (risk_free_rate.transpose().array() * price.row(t-1).array() * dt) + (price.row(t-1).array() * vol.row(t).array());
        change_in_price_vd.row(t) = (risk_free_rate.transpose().array() * price_variance_reduction.row(t-1).array() * dt) + (price_variance_reduction.row(t-1).array() * vol_vd.row(t).array());
        
        price.row(t) = price.row(t-1) + change_in_price.row(t);
        price_variance_reduction.row(t) = price_variance_reduction.row(t-1) + change_in_price_vd.row(t);
    }
   
    return {price, price_variance_reduction};
}


// Private Method 2 ---------------------------------------------------------
Eigen::MatrixXd Multidimensional_Risk_Neutral_Engine::Terminal_prices(int no_simulations){

    /* Eigen::MatrixXd simulation;

    for (int i = 0; i < no_simulations; i++){

        simulation.row(i) = Multidimensional_GBM(correlation_matrix); 

    } */
    
    Eigen::MatrixXd out(2, 2);
    
    out.setZero(); 


    return out;

}

// Private Method 3 ---------------------------------------------------------
//double Multidimensional_Risk_Neutral_Engine::Option_Calculation(){

    /*
    std::normal_distribution<double> standard_norm(0, 1);

    int M = volatility_realised.rows(); // number of assets/options
    int D = volatility_realised.cols(); // number of Brownian drivers
    

    Eigen::VectorXd price(M);

    d_2 = (1 / (volatility_implied * sqrt(tau))) * (log(price.array() / strike.array()) + (risk_free_rate - (0.5 * volatility_implied.array().square()).transpose().colwise()))
    
    double option = prices * stats::normal_cdf(d_1) - strike * exp(-risk_free_rate * tau) * stats::normal_cdf(d_2);
    */
//}

// Public Method 1 ---------------------------------------------------------
Eigen::MatrixXd Multidimensional_Risk_Neutral_Engine::Risk_Neutral_MultiDim_DHE(bool call){
    
    /* Eigen::VectorXd out = Z_scores(true);//Brownian_Mot(discretisation, Time, std::nullopt); 

    Eigen::MatrixXd xxx(volatility_implied.rows(), volatility_implied.cols());
    xxx.col(0) = out;
    xxx.col(1) = out; */
    auto[A, B] = Multidimensional_GBM(std::nullopt);

    return B; 
}

