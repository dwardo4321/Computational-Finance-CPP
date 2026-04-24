#include <iostream>
#include <Eigen/Dense>

int main() {
    Eigen::Matrix2d A;
    A << 4, 1,
         1, 3;

    Eigen::LLT<Eigen::Matrix2d> llt(A);
    Eigen::Matrix2d L = llt.matrixL();

    std::cout << "A =\n" << A << "\n\n";
    std::cout << "L =\n" << L << "\n\n";
    std::cout << "L * L^T =\n" << L * L.transpose() << "\n";
}