#include <iostream>
#include <Eigen/Dense>
#include <cmath>
#include <numbers>
#include <vector>
#include <random>
#include <optional>
#include <string>
#include <sstream>

std::random_device rd;
std::mt19937_64 gen1(rd()); 

class Asset_Option_Pricing{
    
    private:

        Eigen::MatrixXd Brownian_path_generator(std::optional<Eigen::MatrixXd> correlation_matrix){
            
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
                    delta_BM = sqrt(delta_time) * standard_normal_rv.row(i) * lower_triangular.transpose();
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
        }

        std::pair <Eigen::MatrixXd, Eigen::MatrixXd> GBM_price_path_generator(std::optional<Eigen::MatrixXd> correlation_matrix){ 
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
        };
        
    public:
    
        Eigen::VectorXd strike;     
        Eigen::VectorXd rate;         // fixed rate
        Eigen::VectorXd volatility;   // fixed volatility
        Eigen::VectorXd price_today;
        double Time;                  // time duration
        int dimensions;               // number of assets
        int discretisation_brownian_motion; // number of steps            

            // Constructor 1: default constructor (homogeneous multi-assets)--------------------------------------------------------------------------------------------
            Asset_Option_Pricing(double strike_constr,
                                 double rate_constr,
                                 double volatility_constr,
                                 double price_today_constr,
                                 double Time_constr,
                                 int dimensions_constr,
                                 int discretisation_brownian_motion_constr):

                strike(Eigen::VectorXd::Constant(dimensions_constr, strike_constr)), rate(Eigen::VectorXd::Constant(dimensions_constr, rate_constr)),
                volatility(Eigen::VectorXd::Constant(dimensions_constr, volatility_constr)), price_today(Eigen::VectorXd::Constant(dimensions_constr, price_today_constr)),
                Time(Time_constr), dimensions(dimensions_constr), discretisation_brownian_motion(discretisation_brownian_motion_constr) {}
            
            // Constructor 2: member initializer list (heterogeneous multi-assets) --------------------------------------------------------------------------------------------
            Asset_Option_Pricing(const Eigen::VectorXd& strike_constr,
                                 const Eigen::VectorXd& rate_constr,
                                 const Eigen::VectorXd& volatility_constr,
                                 const Eigen::VectorXd& price_today_constr,
                                 double Time_constr,
                                 //int dimensions_constr,
                                 int discretisation_brownian_motion_constr):

                strike(strike_constr), rate(rate_constr), volatility(volatility_constr), price_today(price_today_constr),
                Time(Time_constr), dimensions(static_cast<int>(strike_constr.size())), discretisation_brownian_motion(discretisation_brownian_motion_constr) 
                {
                    if (rate_constr.size() != strike_constr.size() ||
                        volatility_constr.size() != strike_constr.size() ||
                        price_today_constr.size() != strike_constr.size()){
                            throw std::runtime_error("All input vectors must have the same size.");
                        }

                    if (discretisation_brownian_motion_constr < 2){
                            throw std::runtime_error("There should be more than 2 steps");
                        }
                    
                    if (Time_constr < 0){
                            throw std::runtime_error("Time should be greater than 0");
                        }
                }
                

        // Class Method 1 -------------------------------------------------------------------------------------------
        Eigen::MatrixXd GBM_price_path(std::optional<Eigen::MatrixXd> correlation_matrix){
            Eigen::MatrixXd out(discretisation_brownian_motion, dimensions);
            if(correlation_matrix.has_value()){
                auto[A, B] = GBM_price_path_generator(correlation_matrix.value());
                return A;
            }else{
                auto[A, B] = GBM_price_path_generator(std::nullopt);
                return A;
            }    
        }

        // Class Method 2 -------------------------------------------------------------------------------------------
        struct Option_output
        {
            Eigen::VectorXd sample_mean;
            Eigen::VectorXd sample_variance;
            Eigen::VectorXd sample_standard_error;
            std::string confidence_intervals;
        };

        Option_output Monte_Carlo_option_pricer(int number_of_iterations , std::optional<bool> variance_reduction, std::optional<Eigen::MatrixXd> correlation_matrix){
            Option_output output{};
            
            Eigen::MatrixXd prices(number_of_iterations, dimensions);
            Eigen::MatrixXd prices_variance_reduction(number_of_iterations, dimensions);

            Eigen::MatrixXd pay_off(number_of_iterations, dimensions);
            Eigen::MatrixXd pay_off_variance_reduction(number_of_iterations, dimensions);  

            Eigen::MatrixXd discounted_pay_off(number_of_iterations, dimensions);
                   
                if (correlation_matrix.has_value()){
                    for (int i = 0; i < number_of_iterations; i++){
                        
                        auto[A, B] = GBM_price_path_generator(correlation_matrix.value());
                        prices.row(i) = A(Eigen::last, Eigen::all);
                        prices_variance_reduction.row(i) = B(Eigen::last, Eigen::all);
                        
                            for (int j = 0; j < dimensions; j++){ 
                                prices(i, j) - strike(j) > 0? pay_off(i, j) = (prices(i, j) - strike(j)) * exp(-rate(j) * Time):  pay_off(i, j) = 0;  
                                prices_variance_reduction(i, j) - strike(j) > 0? pay_off_variance_reduction(i, j) = (prices_variance_reduction(i, j) - strike(j)) * exp(-rate(j) * Time):  pay_off_variance_reduction(i, j) = 0; 
                                discounted_pay_off(i, j) = 0.5 * (pay_off(i, j) + pay_off_variance_reduction(i, j));
                            }  
                    }    
                }
                else{
                    for (int i = 0; i < number_of_iterations; i++){
                        
                        auto[A, B] = GBM_price_path_generator(std::nullopt);
                        prices.row(i) = A(Eigen::last, Eigen::all);
                        prices_variance_reduction.row(i) = B(Eigen::last, Eigen::all);
                        
                            for (int j = 0; j < dimensions; j++){ 
                                prices(i, j) - strike(j) > 0? pay_off(i, j) = (prices(i, j) - strike(j)) * exp(-rate(j) * Time):  pay_off(i, j) = 0;  
                                prices_variance_reduction(i, j) - strike(j) > 0? pay_off_variance_reduction(i, j) = (prices_variance_reduction(i, j) - strike(j)) * exp(-rate(j) * Time):  pay_off_variance_reduction(i, j) = 0; 
                                discounted_pay_off(i, j) = 0.5 * (pay_off(i, j) + pay_off_variance_reduction(i, j));
                            }
                    }
                }
            
            Eigen::VectorXd sample_mean(dimensions); sample_mean.setZero();
            Eigen::VectorXd sample_variance(dimensions); sample_variance.setZero();
            Eigen::VectorXd sample_standard_error(dimensions); sample_standard_error.setZero();
            Eigen::VectorXd ci_lower_bound(dimensions); ci_lower_bound.setZero();
            Eigen::VectorXd ci_upper_bound(dimensions); ci_upper_bound.setZero();
            
            
            // SAMPLE MEAN ------------------------------------------------------------------------------------
            for (int j = 0; j < dimensions; j++){
                for (int i = 0; i < number_of_iterations; i++){
                    sample_mean(j) += discounted_pay_off(i, j);    
                }
                sample_mean(j) = sample_mean(j) / static_cast<double>(number_of_iterations);
            }
            output.sample_mean = sample_mean;  
            
            // SAMPLE VARIANCE --------------------------------------------------------------------------------
            for (int j = 0; j < dimensions; j++){
                for (int i = 0; i < number_of_iterations; i++){
                    sample_variance(j) += pow((discounted_pay_off(i, j) - sample_mean(j)), 2);    
                }
                sample_variance(j) = sample_variance(j) / static_cast<double>(number_of_iterations - 1);
            }
            output.sample_variance = sample_variance;
            
            // STANDARD ERROR ------------------------------------------------------------------------------------
            for (int i = 0; i < dimensions; i++){
                sample_standard_error(i) = sqrt(sample_variance(i) / static_cast<double>(number_of_iterations));
            }
            output.sample_standard_error = sample_standard_error;

            // CONFIDENCE INTERVALS -----------------------------------------------------------------------------
            std::stringstream confidence_intervals;

            for(int i = 0; i < dimensions; i++){
                ci_lower_bound(i) = sample_mean(i) - 1.96 * sample_standard_error(i);   
                ci_upper_bound(i) = sample_mean(i) + 1.96 * sample_standard_error(i);
                confidence_intervals << "[" << ci_lower_bound(i) << ": " << ci_upper_bound(i) << "]\n";
            }
            output.confidence_intervals = confidence_intervals.str();
            return output;
        }
};

int main(){

    //std::cout << "--------------------------------------------------------\n";

    //Asset_Option_Pricing asset_3 = Asset_Option_Pricing(5000, 0.14, 0.25, 5100, 1.5, 5, 10);
    //std::cout << asset_3.Brownian_path_generator({}) << '\n';  //asset_3.Path_generator(std::nullopt)

    std::cout << "--------------------------------------------------------\n";

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
    Asset_Option_Pricing asset_4 = Asset_Option_Pricing(strike, rate, volatility, price_today, 2, 500);
    std::cout << asset_4.Monte_Carlo_option_pricer(1000, {}, correlation).confidence_intervals << '\n';
    //std::cout << asset_4.GBM_price_path(correlation) << '\n';

    return 0;
    }