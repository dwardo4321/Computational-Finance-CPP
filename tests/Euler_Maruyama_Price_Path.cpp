#include <iostream>
#include <random>
#include <Eigen/Dense>

std::random_device rd;
std::mt19937_64 gen1(rd());

Eigen::MatrixXd Path_generator(double S_0, double mu, double sigma, int T, int N_steps, int N_paths){

    double dt = T / static_cast<double> (N_steps);
    double v = mu - 0.5 * pow(sigma, 2);
    double nudt = v * dt;
    double sidt = sigma * pow(dt, 0.5); 
    
    Eigen::MatrixXd S_step_path(N_steps, N_paths);
    std::normal_distribution<double> snrv(0, 1);

    for (int j = 0; j < N_paths; j++)
    {   
        for (int i = 0; i < N_steps; i++)
        {
            if (i == 0)
            {
                S_step_path(i, j) = S_0 * exp(nudt + sidt * snrv(gen1));
            }else{
                S_step_path(i, j) = S_step_path(i-1, j) * exp(nudt + sidt * snrv(gen1));
            }                 
        }
    
    }
    
    return S_step_path;

} 

int main(){
    std::cout << Path_generator(1500, 0.18, 0.2, 2, 1000, 2);
    return 0;
}