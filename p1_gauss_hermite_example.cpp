#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <Eigen/Dense>

using namespace Eigen;

// Returns (nodes, weights) for Gauss–Hermite adapted to N(mu, sigma2).
inline std::pair<std::vector<double>, std::vector<double>>
gauss_hermite(double mu, double sigma2, int N)
{
    MatrixXd J = MatrixXd::Zero(N, N);
    for (int k = 0; k < N - 1; ++k) {
        double beta = std::sqrt((k + 1) / 2.0);
        J(k, k + 1) = beta;
        J(k + 1, k) = beta;
    }

    SelfAdjointEigenSolver<MatrixXd> es(J);
    VectorXd x = es.eigenvalues();              // nodes for weight e^{-x^2}
    MatrixXd V = es.eigenvectors();
    VectorXd w = std::sqrt(M_PI) * V.row(0).array().square().matrix();

    std::vector<double> nodes(N), weights(N);
    for (int i = 0; i < N; ++i) {
        double xi = x(i);
        double wi = w(i) / std::sqrt(M_PI);     // normalize to sum to 1
        nodes[i]   = std::sqrt(2.0 * sigma2) * xi + mu;
        weights[i] = wi;
    }
    return {nodes, weights};
}

int main()
{
    double mu = 0.1;
    double sigma2 = 0.2;
    int N = 10;

    auto result = gauss_hermite(mu, sigma2, N);
    const auto &nodes   = result.first;
    const auto &weights = result.second;

    double approx = 0.0;
    for (int i = 0; i < N; ++i) {
        approx += weights[i] * std::exp(1.245 * nodes[i]);    // approx E[e^(1.245X)]
    }
    double exact = std::exp(mu + 0.5 * sigma2);

    std::cout << "Approx: " << approx << "\n";
    std::cout << "Exact : " << exact  << "\n";

    return 0;
}
