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
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//--------------------------------[PART 2a: RNG FOR STANDARD NORMAL RVs]------------------------------------------
//------------------------------------------[Box-Muller]----------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
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

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//--------------------------------[PART 2b: RNG FOR STANDARD NORMAL RVs]------------------------------------------
//----------------------------------------[Marsaglia Method]-----------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

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

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//--------------------------------[PART 3: RNG FOR STANDARD NORMAL RVs]------------------------------------------
//--------------------------------------[And mapping to prices]--------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//-----------------[PART 4: Multivariate terminal prices at a single time T with GBM ]---------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//-------------------------[PART 5: MC option pricing with variance reduction]-----------------------------------
//----------------------------------------[Antithetic Method]----------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//------------------------------[PART 6: Generating Sample Paths for GBM]----------------------------------------
//--------------------------------------[Euler–Maruyama scheme]--------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//------------------------------[PART 7: CSV export for generated paths]-----------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

/* 
 * Write simulated GBM paths to a CSV file.
 * - 'paths' is an N_steps x N_paths matrix (row = time step, col = path).
 * - First CSV column is time t, remaining columns are path0, path1, ...
 * - File can be loaded in Python/Excel for plotting/analysis.
 */

void write_paths_to_csv(
    const std::string& filename,
    const Eigen::MatrixXd& paths,
    double T
) {
    std::cout << "write_paths_to_csv CWD is: " << fs::current_path() << "\n";

    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Could not open file " << filename << " for writing.\n";
        return;
    }

    int N_steps = paths.rows();
    int N_paths = paths.cols();
    double dt   = T / static_cast<double>(N_steps);

    file << "t";
    for (int p = 0; p < N_paths; ++p) {
        file << ",path" << p;
    }
    file << "\n";

    for (int i = 0; i < N_steps; ++i) {
        double t = (i+1) * dt;
        file << t;
        for (int p = 0; p < N_paths; ++p) {
            file << "," << paths(i, p);
        }
        file << "\n";
    }

    file.close();
    std::cout << "Wrote paths to " << filename << "\n";
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//--------------------------------[PART 8: Simulating Brownian Bridge]-------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------


   /*
    Brownian Bridge uses recursive bisection of time intervals.
    We start with the endpoints W(0) and W(T), then repeatedly fill in
    midpoints: T/2, T/4 & 3T/4, T/8 & 3T/8 & 5T/8 & 7T/8, etc.

    After each bisection step, the number of intervals doubles:
        1 → 2 → 4 → 8 → … → N_steps.

    Therefore, if we want exactly N_steps time intervals (typically chosen
    as a power of 2), we need:

        N_bisections = log2(N_steps)

    This tells us how many times we must split intervals in half until the
    full time grid is constructed. Using powers of 2 makes the Brownian
    bridge algorithm simple and efficient.
    */

    //return W_Samples;

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
    std::cout << "--------MC option pricing with variance reduction-------\n";

    /*
    double K, r, v, S_0;
    int T, Nit;

    std::cout << "Enter Strike Price K: ";
    std::cin >> K;
    std::cout << "Enter rate r: ";
    std::cin >> r;
    std::cout << "Enter volatility v: ";
    std::cin >> v;
    std::cout << "Enter Price today S_0: ";
    std::cin >> S_0;
    std::cout << "Enter maturity T: ";
    std::cin >> T;
    std::cout << "Enter number of simulations Nit: ";
    std::cin >> Nit; 

    four_out calc = MC_option_price(K, r, v, S_0, T, Nit);
    std::cout << "£" <<std::setprecision(2) << std::fixed << calc.sample_mean << " with a variance of " << calc.var_estimatorb << " and s.e " << calc.se << '\n';
    std::cout << "The 95% CI: [" << calc.ci_lower << " : " << calc.ci_upper << "]" << '\n';
    */

    std::cout << "--------------------------------------------------------\n";
    std::cout << "------Multivariate terminal prices at a single time T with GBM-----\n";
   
    /*
    int d, N_i;
    std::cout << "Enter number of assets (d): ";
    std::cin >> d;
    std::cout << "Enter simulations per asset (N_i): ";
    std::cin >> N_i;
    std::cout << Price_gen(d, N_i);
    */

    std::cout << "--------------------------------------------------------\n";
    std::cout << "-----------mapping normal RVs to prices-----------------\n";
   
    Eigen::MatrixXd Corr(5, 5);
    Corr <<
        1.00, 0.40, 0.35, 0.30, 0.25,
        0.40, 1.00, 0.32, 0.28, 0.30,
        0.35, 0.32, 1.00, 0.38, 0.33,
        0.30, 0.28, 0.38, 1.00, 0.36,
        0.25, 0.30, 0.33, 0.36, 1.00;

    // Rates (per year) for 5 assets
    Eigen::VectorXd r(5);
    r << 0.03, 0.025, 0.04, 0.035, 0.02;

    // Volatilities (per sqrt(year)) for 5 assets
    Eigen::VectorXd v(5);
    v << 0.15, 0.20, 0.18, 0.22, 0.17;

    // Initial prices for 5 assets
    Eigen::VectorXd S0(5);
    S0 << 100.0, 120.0, 95.0, 140.0, 80.0;

    int T;
    T = 10;
    std::cout << simulate_terminal_prices(r, v, Corr, T, S0);

    std::cout << "--------------------------------------------------------\n";
    std::cout << "-----------Generating Sample Paths for GBM--------------\n";

    double S_0    = 1000;
    double mu     = 0.03;
    double sigma  = 0.1;
    int Time      = 5; 
    int N_steps   = 1000;
    int N_paths   = 1000;

    // Generate paths
    Eigen::MatrixXd paths = Path_generator(S_0, mu, sigma, Time, N_steps, N_paths);

    // Write to CSV (absolute path so you KNOW where it goes)
    std::string out_file = "C:/Users/tc5608f/Downloads/cpp_output/gbm_paths_1.csv";
    std::cout << "Main CWD is: " << fs::current_path() << "\n";
    write_paths_to_csv(out_file, paths, Time);

    /*  PYTHON CODE FOR PLOT
        import pandas as pd
        import matplotlib.pyplot as plt

        plt.figure(figsize=(20,8))
        df = pd.read_csv("gbm_paths_1.csv")
        for col in df.columns[1:]:  # skip t column
            plt.plot(df["t"], df[col], alpha=0.8)
        plt.xlabel("t")
        plt.ylabel("S_t")
        plt.title("GBM sample paths")
        plt.show()
    */

 
    int N_bisections = 3;  // for example
    double Tt = 1.0;

    Eigen::VectorXd path = brownian_bridge(N_bisections, Tt);

    



    return 0;
}