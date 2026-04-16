#include <iostream>
#include <Eigen/Dense>
#include <cmath>
#include <numbers>
#include <vector>
#include <random>
#include <optional>

std::random_device rd;
std::mt19937_64 gen1(rd()); 

class Asset_Option_Pricing{
    
    private: // access modifier
        double Time;                  // time duration
        int dimensions;               // number of assets
        int discretisation_price;
        int discretisation_brownian_motion; // number of steps 

            // Class Method 1 -------------------------------------------------------------------------------------------
            Eigen::MatrixXd Path_generator(int dimensions, int discretisation_brownian_motion, int Time, std::optional<Eigen::MatrixXd> correlation_matrix = std::nullopt){
                
                Eigen::MatrixXd out; 
                
                if(correlation_matrix.has_value()){
                    
                } else {

                }

                return out; 
            }
            
    public: 
        Eigen::VectorXd stike;     
        Eigen::VectorXd rate;         // fixed rate
        Eigen::VectorXd volatility;   // fixed volatility
        Eigen::VectorXd price_today;             

            // Class Method 2 -----------------------------------------------------------------------------------------
            Eigen::MatrixXd Price_generator(int dimensions, int discretisation_price){   // Generate prices using GBM

                Eigen::MatrixXd Matri(discretisation_price, dimensions);   // discretisation rows and 0 columns

                double delta_time = static_cast<double>(Time) / discretisation;
        
                std::normal_distribution<double> standard_norm(0, 1);

                    for(int j = 0; j < dimensions; j++){ 
                        
                        Matri(0, j) = price_today;

                        for(int i = 1; i < discretisation; i++){

                            Matri(i, j) = Matri(i - 1, j) * exp(((rate - 0.5*(pow(volatility, 2))) * delta_time) + volatility*static_cast<double>(sqrt(delta_time))*standard_norm(gen1));   //GBM                              
                        
                        };
                    };

                return Matri;
            };


            // Constructor 1: default constructor --------------------------------------------------------------------------------------------
            Asset_Option_Pricing(): stike(0), rate(0), volatility(0), price_today(0), Time(0){}

            // Constructor 2: member initializer list --------------------------------------------------------------------------------------------
            Asset_Option_Pricing(double strike_constr, double rate_constr, double volatility_constr, double price_today_constr, double Time_constr):

                stike(strike_constr), rate(rate_constr), volatility(volatility_constr), price_today(price_today_constr), Time(Time_constr) {}
            
};

/*
                class Asset_Option_Pricing{
                    protected:
                        double K;     
                        double r;     
                        double v;     
                        double S_0;  
                        int T;        

                    private:
                        std::string Company;
                        int Age;

                    public:
                        employee(std::string name, std::string company, int age){
                            Name = name;
                            Company = company;
                            Age = age;
                        }

                        void intro(){
                            std::cout << "Name - " << Name << '\n';
                            std::cout << "Company - " << Company << '\n';
                            std::cout << "Age - " << Age << '\n';
                        }

                        void askforpromotion(){
                            if (Age > 30)
                                std::cout << Name << " got promoted! \n";
                            else
                                std::cout << Name << ", sorry no promotion for you! \n";
                        }
                };
*/

int main(){

    Asset_Option_Pricing asset_1;
    /*
    asset_1.price_today = 1500;
    asset_1.rate = 0.09;
    asset_1.stike = 1486;
    asset_1.Time = 2;
    asset_1.volatility = 0.15;
    */
    std::cout << asset_1.Price_generator(5, 5) << '\n';

    std::cout << "--------------------------------------------------------\n";

    Asset_Option_Pricing asset_2;
    /*
    asset_2.price_today = 100;
    asset_2.rate = 0.15;
    asset_2.stike = 86;
    asset_2.Time = 1;
    asset_2.volatility = 0.25;
    */
    std::cout << asset_2.Price_generator(5, 5) << '\n';

    std::cout << "--------------------------------------------------------\n";

    Asset_Option_Pricing asset_3 = Asset_Option_Pricing(5000, 0.14, 0.25, 5100, 1.5);
    std::cout << asset_3.Price_generator(5, 5) << '\n';
    
    return 0;
}