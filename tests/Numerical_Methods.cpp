#include <iostream>
#include <random>
#include <cmath>
#include <numbers>
#include <vector>
#include <iomanip>
#include <Eigen/Dense>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;  //csv trans


// If you want a different sequence each run
     std::random_device rd;
     std::mt19937_64 gen1(rd());   
     std::mt19937_64 gen2(rd()); 
// Else
//std::mt19937_64 gen();

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//-------------------------[PART 1: MONTE CARLO INTEGRATION (exponential func)]----------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

double MC_integration(int N){ 
    // Monte Carlo integration
    double uniform_vector[N] = {};

    for (int i = 0; i < N; i++){
        uniform_vector[i] = gen1()/static_cast<double>(gen1.max());
        // std::cout << "uniform_vector[" << i+1 << "] = " << uniform_vector[i] << '\n';     
    }
    
    // Vector of g(U)
    double func_values = 0;

    for(int j = 0; j < N; j++){
        func_values += exp(uniform_vector[j]);      
    }

    double MC_approx = func_values/N;
    return MC_approx;
}
// Box-Muller --------------------------------------------------------------------
struct Norm4 {
    std::vector<double> X;
    std::vector<double> Y;
    std::vector<double> std_norm1;
    std::vector<double> std_norm2;
};

Norm4 Standard_Norm(int N){ 
    
    Norm4 out;

    // Uniform RV: 1 (Improvised)
    std::vector<double> uniform_vector1;
    for (int i = 0; i < N; i++){
        uniform_vector1.push_back(gen1()/static_cast<double>(gen1.max()));
        // std::cout << "uniform_vector[" << i+1 << "] = " << uniform_vector[i] << '\n';     
    }

    // Uniform RV: Built-in
    std::uniform_real_distribution<double> uniform(0, 1);
    std::vector<double> uniform_vector2;
    uniform_vector2.reserve(N);
    for (int i = 0; i < N; i++){
        uniform_vector2.push_back(uniform(gen1));
        // std::cout << "uniform_vector[" << i+1 << "] = " << uniform_vector[i] << '\n';     
    }

    // Standarn Normal RV: Box–Muller
    std::vector<double> X, Y;
    constexpr double PI = 3.14159265358979323846;
    out.X.reserve(N), out.Y.reserve(N);
    for(int i = 0; i < N; i++){
        
        out.X.push_back(sqrt(-2*log(uniform_vector1[i])) * cos(2*PI*uniform_vector2[i]));
        out.Y.push_back(sqrt(-2*log(uniform_vector1[i])) * sin(2*PI*uniform_vector2[i]));
    }
    
    // Standarn Normal RV: Built-in
    std::normal_distribution<double> Std_Norm_RNG(0, 1);
    std::vector<double> std_norm1, std_norm2;
    out.std_norm1.reserve(N); 
    out.std_norm2.reserve(N); 
    for(int i = 0; i < N; i++){
        out.std_norm1.push_back(Std_Norm_RNG(gen1));
        out.std_norm2.push_back(Std_Norm_RNG(gen2));
    }
    /*
        Array: fixed-size, low-level, no methods, can decay to pointer. (Way 1)
        Vector: resizable, safe, high-level, has methods, manages memory for you. (Way 2)
    */

    // return {std_norm1, std_norm2};
    // return {X, Y};
    return {out};
}

// Marsaglia Method ----------------------------------------------------------------

   std::pair<double,double> marsaglia_method(std::mt19937_64 &rng) {
    std::uniform_real_distribution<double> unif(0, 1);
    double v1, v2, s;
    do {
        double u1 = unif(rng);
        double u2 = unif(rng);
        v1 = 2 * u1 - 1;
        v2 = 2 * u2 - 1;

        s = v1 * v1 + v2 * v2;
    } while (s <= 0 || s > 1);

    double factor = std::sqrt(-2 * log(s) / s);
    double z1 = v1 * factor;
    double z2 = v2 * factor;
    return {z1, z2};
    }

Eigen::VectorXd brownian_bridge(int N_bisections, double T)
{
    // Total number of time steps is 2^N_bisections
    int N_steps  = 1 << N_bisections;     // 2^N_bisections
    int N_points = N_steps + 1;           // indices 0..N_steps

    Eigen::VectorXd W_Samples(N_points);

    std::cout << "N_bisections = " << N_bisections
              << ", N_steps = " << N_steps
              << ", N_points = " << N_points << "\n";

    // 1) Set endpoints
    W_Samples(0) = 0.0;

    std::normal_distribution<double> nd_T(0.0, std::sqrt(T));
    W_Samples(N_steps) = nd_T(gen1);

    std::cout << "W(0) = " << W_Samples(0) << "\n";
    std::cout << "W(T) = W(" << T << ") = " << W_Samples(N_steps) << "\n\n";

    // Interval struct
    struct Interval {
        int i;
        int j;
    };

    // Stack of intervals to process
    std::vector<Interval> stack;
    stack.push_back({0, N_steps});

    // Standard normal for bridge increments
    std::normal_distribution<double> nd_std(0.0, 1.0);

    // 2) Bisection loop
    while (!stack.empty()) {
        Interval interval = stack.back();
        stack.pop_back();

        int i = interval.i;
        int j = interval.j;

        // If there's no space for a midpoint, skip
        if (j - i <= 1) {
            continue;
        }

        int m = (i + j) / 2;  // midpoint index

        // Times corresponding to these indices
        double t_i = (double)i / (double)N_steps * T;
        double t_j = (double)j / (double)N_steps * T;
        double t_m = (double)m / (double)N_steps * T;

        // Brownian bridge conditional mean and variance
        double mean = ((t_j - t_m) / (t_j - t_i)) * W_Samples(i)
                    + ((t_m - t_i) / (t_j - t_i)) * W_Samples(j);

        double var  = (t_m - t_i) * (t_j - t_m) / (t_j - t_i);

        double Z = nd_std(gen1);
        double Wm = mean + std::sqrt(var) * Z;
        W_Samples(m) = Wm;

        // ---- DEBUG PRINTS HERE ----
        std::cout << "Interval [" << i << ", " << j << "]"
                  << "  -> midpoint index m = " << m << "\n";
        std::cout << "  t_i = " << t_i << ", t_m = " << t_m
                  << ", t_j = " << t_j << "\n";
        std::cout << "  W(" << t_i << ") = " << W_Samples(i)
                  << ", W(" << t_j << ") = " << W_Samples(j) << "\n";
        std::cout << "  mean = " << mean
                  << ", var = " << var << "\n";
        std::cout << "  Draw Z = " << Z
                  << "  ->  W(" << t_m << ") = " << Wm << "\n\n";

        // Push the two subintervals for further splitting
        stack.push_back({i, m});
        stack.push_back({m, j});
    }

    std::cout << "Final Brownian path values:\n";
    for (int k = 0; k <= N_steps; ++k) {
        double t = (double)k / (double)N_steps * T;
        std::cout << "  k = " << k
                  << ", t = " << t
                  << ", W(t) = " << W_Samples(k) << "\n";
    }

    return W_Samples;
}



//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------[PART 8: MAIN FUNCTION]-------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

int main(){

    std::cout << "--------------Monte-Carlo Integration------------------\n";

    int N[] = {10, 100, 1000, 10000, 100000};
    auto [X, Y, std_norm1, std_norm2] = Standard_Norm(N[0]);

    for (int i = 0; i < sizeof(N)/sizeof(N[0]); i++){
        MC_integration(N[i]);
        std::cout << "The MC approximate for " << N[i] << " uniform RVs is " << MC_integration(N[i]) << '\n';
    }

    std::cout << "--------------------------------------------------------\n";
    std::cout << "--------------Box-Muller method output------------------\n";

    for (int j = 0; j < 5; j++){
        std::cout << std::setprecision(4) << std::fixed << X[j] << "   ;   " << Y[j] << "   ;   " << std_norm1[j] << "   ;   " << std_norm2[j] << "\n";
    }
    
    std::cout << "--------------------------------------------------------\n";
    std::cout << "--------------Marsaglia method output-------------------\n";

    for (int i = 0; i < 5; ++i) {
        auto [z1, z2] = marsaglia_method(gen1);  // rng passed BY REFERENCE

        std::cout << "Pair " << i+1 << ": "
                  << "z1 = " << z1 << ", z2 = " << z2 << '\n';
    }

    std::cout << "--------------------------------------------------------\n";
    std::cout << "--------------Brownian Bridge-------------------\n";
 
    int N_bisections = 3;  // for example
    double Tt = 1.0;

    Eigen::VectorXd path = brownian_bridge(N_bisections, Tt);

    return 0;
}