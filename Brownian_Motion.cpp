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

Eigen::MatrixXd simulate_terminal_prices(
    Eigen::VectorXd r,
    Eigen::VectorXd v,
    Eigen::MatrixXd Corr,
    int T,
    Eigen::VectorXd S0
    )
{
    Eigen::MatrixXd mu(r.size(), 1);
    Eigen::MatrixXd Cov(r.size(), r.size());

    // 1) Mean vector mu_i = (r_i - 0.5 * v_i^2) * T
    for(int i = 0; i < v.size(); i++)
    {
        mu(i) = (r(i) - 0.5 * pow(v(i), 2)) * T; 
    };

    // 2) Covariance matrix Cov_ij = Corr_ij * v_i * v_j
    for (int j = 0; j < v.size(); j++)
        {
            for(int k = 0; k < v.size(); k++)
                {
                    Cov(j, k) = Corr(j, k) * v(j) * v(k) * T; 
                };
        };

    // 3) Cholesky: Cov = L L^T
    Eigen::LLT<Eigen::MatrixXd> llt(Cov);
    Eigen::MatrixXd L = llt.matrixL();

    // 4) Generate Z ~ N(0, I_d)   
    std::normal_distribution<double> standard_norm(0, 1);
    Eigen::MatrixXd Z(r.size(), 1);
    for (int i = 0; i < r.size(); i++)
    {
        Z(i) = standard_norm(gen1);
    }
    
    // Generate 
    Eigen::MatrixXd X;
    X = mu + L * Z;
    
    // 6) Map to prices: S_T = S_0 * exp(X)
    Eigen::MatrixXd ST(r.size(), 1);
    for (int i = 0; i < r.size(); ++i) {
        ST(i) = S0(i) * std::exp(X(i));
    }

    return ST;

}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//-----------------[PART 4: Multivariate terminal prices at a single time T with GBM ]---------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

Eigen::MatrixXd Price_gen(int d, int N_i){   

    // Generate prices using GBM
    
    // Eigen::MatrixXd M; completely empty matrix 0x0
    Eigen::MatrixXd Matri(N_i, 0);   // N_i rows, 0 columns
    

    std::normal_distribution<double> standard_norm(0, 1);

    while(Matri.cols() < d){ 
        Eigen::MatrixXd S_t(N_i, 1);
        double K, r, v, S_0;
        int T;

        std::cout << "--------------------------------------------------------\n";
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
        
            for(int i = 0; i < N_i; i++)
                {
                    S_t(i) = S_0 * exp(((r - 0.5*(pow(v, 2))) * T) + v*static_cast<double>(sqrt(T))*standard_norm(gen1));   //GBM
                }
        Matri.conservativeResize(N_i, Matri.cols() + 1);
        Matri.col(Matri.cols() - 1) = S_t;         
    };

    return Matri;

}




//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//-------------------------[PART 5: MC OPTION PRICING WITH VARIANCE REDUCTION]-----------------------------------
//----------------------------------------[Antithetic Method]----------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

struct four_out
{
    double sample_mean;
    double se;
    double var_estimatorb;
    double ci_lower;
    double ci_upper;
};


four_out MC_option_price(double K, double r, double v, double S_0, int T, int Nit){ 

    four_out out{};
    std::normal_distribution<double> Std_Norm_RNG(0, 1);
    
    std::vector<double> S_T;
    std::vector<double> S_T_vd;  // for variance reduction
    std::vector<double> PV;
    std::vector<double> PV_norm;  
    std::vector<double> PV_vd;    // for variance reduction
    std::vector<double> normalrv;
    double total_PV = 0;
    S_T.reserve(Nit);
    S_T_vd.reserve(Nit);  // for variance reduction
    PV.reserve(Nit);
    PV_norm.reserve(Nit);
    PV_vd.reserve(Nit);  // for variance reduction
    normalrv.reserve(Nit);

    for(int i = 0; i < Nit; i++){
        normalrv.push_back(Std_Norm_RNG(gen1));
    }
    
    
    for (int i = 0; i < Nit; i++){
        S_T.push_back(S_0 * exp(((r - 0.5*(pow(v, 2))) * T) + v*static_cast<double>(sqrt(T))*normalrv[i]));   //GBM
        S_T_vd.push_back(S_0 * exp(((r - 0.5*(pow(v, 2))) * T) + v*static_cast<double>(sqrt(T))*-normalrv[i]));  // for variance reduction

        S_T[i] - K > 0?  PV_norm.push_back((S_T[i]-K) * exp(-r*T)):  PV_norm.push_back(0);  // Discounted payoff vector
        S_T_vd[i] - K > 0?  PV_vd.push_back((S_T_vd[i]-K) * exp(-r*T)):  PV_vd.push_back(0);  // for variance reduction

        PV.push_back(0.5 * (PV_norm[i] + PV_vd[i]));  // for variance reduction
    }
    

    for (int i = 0; i < Nit; i++){
        total_PV  += PV[i];    
    } 
    double sample_mean = total_PV/Nit;
    out.sample_mean = sample_mean;  // Price estimate (Mean price)
    

    double diffs = 0, var_payoff, se, var_estimatorb;
    for(int j = 0; j < Nit; j++){
        diffs += pow((PV[j] - sample_mean), 2);
    }      
    var_payoff = diffs/static_cast<double>(Nit-1); // variance of pay off 
    
    var_estimatorb = var_payoff/static_cast<double>(Nit); // variance of price estimator
    se = sqrt(var_payoff/static_cast<double>(Nit));  //standard error  of price estimator

    out.se = se;
    out.var_estimatorb = var_estimatorb;
    out.ci_lower = sample_mean - 1.96 * se;   // Price lower
    out.ci_upper = sample_mean + 1.96 * se;   // Price upper
    return out;
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//------------------------------[PART 6: Generating Sample Paths for GBM]----------------------------------------
//--------------------------------------[Euler–Maruyama scheme]--------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

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

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//----------------------------------[CSV export for generated paths]---------------------------------------------
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
    double mu     = 0.15;
    double sigma  = 0.05;
    int Time      = 20; 
    int N_steps   = 10000;
    int N_paths   = 10;

    // Generate paths
    Eigen::MatrixXd paths = Path_generator(S_0, mu, sigma, Time, N_steps, N_paths);

    // Write to CSV (absolute path so you KNOW where it goes)
    std::string out_file = "C:/Users/Staff/Downloads/cpp_output/gbm_paths_1.csv";
    std::cout << "Main CWD is: " << fs::current_path() << "\n";
    write_paths_to_csv(out_file, paths, Time);




    return 0;
}