#include <iostream>
#include <random>
#include <cmath>
#include <numbers>
#include <vector>
#include <iomanip>
#include <Eigen/Dense>

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
//--------------------------------[PART 2: RNG FOR STANDARD NORMAL RVs]------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
struct Norm4 {
    std::vector<double> X;
    std::vector<double> Y;
    std::vector<double> std_norm1;
    std::vector<double> std_norm2;
};

Norm4 Standard_Norm(int N){ 
    
    Norm4 out;

    // Uniform RV;
    // Way 1 (Improvised)
    std::vector<double> uniform_vector1;
    for (int i = 0; i < N; i++){
        uniform_vector1.push_back(gen1()/static_cast<double>(gen1.max()));
        // std::cout << "uniform_vector[" << i+1 << "] = " << uniform_vector[i] << '\n';     
    }

    // Way 2
    std::uniform_real_distribution<double> uniform(0, 1);
    std::vector<double> uniform_vector2;
    uniform_vector2.reserve(N);
    for (int i = 0; i < N; i++){
        uniform_vector2.push_back(uniform(gen1));
        // std::cout << "uniform_vector[" << i+1 << "] = " << uniform_vector[i] << '\n';     
    }

    // Standarn Normal RV;
    // Way 1 (Box–Muller)
    std::vector<double> X, Y;
    constexpr double PI = 3.14159265358979323846;
    out.X.reserve(N), out.Y.reserve(N);
    for(int i = 0; i < N; i++){
        out.X.push_back(sqrt(-2*log(uniform_vector1[i])) * cos(2*PI*uniform_vector2[i]));
        out.Y.push_back(sqrt(-2*log(uniform_vector1[i])) * sin(2*PI*uniform_vector2[i]));
    }
    
    // Way 2
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
//--------------------------------[PART 2: RNG FOR STANDARD NORMAL RVs]------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

/*
#include <Eigen/Dense>
#include <random>
#include <cmath>

// Simulate one draw of terminal prices S_T for d assets under GBM,
// using the multivariate normal for log-returns.
Eigen::VectorXd simulate_terminal_prices(
    const Eigen::VectorXd& S0,      // initial prices S_0 (size d)
    const Eigen::VectorXd& r,       // rates r_i (size d)
    const Eigen::VectorXd& v,       // vols v_i (size d)
    double T,                       // horizon
    const Eigen::MatrixXd& R,       // correlation matrix (d x d)
    std::mt19937_64& gen            // RNG
)
{
    const int d = S0.size();

    // 1) Mean vector mu_i = (r_i - 0.5 * v_i^2) * T
    Eigen::VectorXd mu(d);
    for (int i = 0; i < d; ++i) {
        mu(i) = (r(i) - 0.5 * v(i) * v(i)) * T;
    }

    // 2) Covariance matrix Sigma_ij = rho_ij * v_i * v_j * T
    Eigen::MatrixXd Sigma(d, d);
    for (int i = 0; i < d; ++i) {
        for (int j = 0; j < d; ++j) {
            Sigma(i, j) = R(i, j) * v(i) * v(j) * T;
        }
    }

    // 3) Cholesky: Sigma = L L^T
    Eigen::LLT<Eigen::MatrixXd> llt(Sigma);
    Eigen::MatrixXd L = llt.matrixL();

    // 4) Generate Z ~ N(0, I_d)
    std::normal_distribution<double> nd(0.0, 1.0);
    Eigen::VectorXd Z(d);
    for (int i = 0; i < d; ++i) {
        Z(i) = nd(gen);
    }

    // 5) X = mu + L Z (log-returns)
    Eigen::VectorXd X = mu + L * Z;

    // 6) Map to prices: S_T = S_0 * exp(X)
    Eigen::VectorXd ST(d);
    for (int i = 0; i < d; ++i) {
        ST(i) = S0(i) * std::exp(X(i));
    }

    return ST;
}
*/

Eigen::MatrixXd simulate_terminal_prices(
    Eigen::VectorXd r,
    Eigen::VectorXd v,
    Eigen::MatrixXd Corr;
    int T
){
    Eigen::VectorXd mu;
    Eigen::MatrixXd Cov(r.size(), r.size());

    for(int i = 0; i < v.size(); i++)
    {
        mu(i) = (r(i) - 0.5 * pow(v(i), 2)) * T; 
    };

    for (int j = 0; j < v.size(); j++)
        {
            for(int k = 0; k < v.size(); k++)
                {
                    Cov(j, k) = Corr(j, k) * v(j) * v(k); 
                };
        };

    // 3) Cholesky: Cov = L L^T
    Eigen::LLT<Eigen::MatrixXd> llt(Cov);
    Eigen::MatrixXd L = llt.matrixL();

    std::normal_distribution stdnorm(0, 1);


    







}












//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//-----------------[PART 2: Multivariate terminal prices at a single time T with GBM ]---------------------------
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
//-------------------------[PART 3: MC OPTION PRICING WITH VARIANCE REDUCTION]-----------------------------------
//---------------------------------------------------------------------------------------------------------------
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
//---------------------------------------[PART 4: MAIN FUNCTION]-------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

int main(){
    int N[] = {10, 100, 1000, 10000, 100000};
    auto [X, Y, std_norm1, std_norm2] = Standard_Norm(N[0]);

    for (int i = 0; i < sizeof(N)/sizeof(N[0]); i++){
        MC_integration(N[i]);
        std::cout << "The MC approximate for " << N[i] << " uniform RVs is " << MC_integration(N[i]) << '\n';
    }

    std::cout << "--------------------------------------------------------\n";
    std::cout << "----------Standard Normal Random Variables--------------\n";

    for (int j = 0; j < 5; j++){
        std::cout << std::setprecision(4) << std::fixed << X[j] << "   ;   " << Y[j] << "   ;   " << std_norm1[j] << "   ;   " << std_norm2[j] << "\n";
    }
    
    std::cout << "--------------------------------------------------------\n";
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

    int d, N_i;
    std::cout << "Enter number of assets (d): ";
    std::cin >> d;
    std::cout << "Enter simulations per asset (N_i): ";
    std::cin >> N_i;
    std::cout << Price_gen(d, N_i);

    return 0;
}