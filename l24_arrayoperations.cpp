#include <iostream>
#include <random>
#include <ctime>

std::mt19937 gen(static_cast<unsigned>(std::time (nullptr)));

int main(){

    constexpr int n = 20;
    double prices[n] = {};
    double num;

    for (int i = 0; i < n; ++i){
        num = gen() / (static_cast<double>(gen.max()) + 1);
        prices[i] = num;
    } 

    std::fill(prices, prices + 5, 0.00);
    std::fill(prices + 5, prices + 10, 1.00);
    std::fill(prices + 10, prices + 15, 2.00);

    for (int i = 0; i < n; ++i){
        std::cout << "Prices[" << i << "] = " << prices[i] << "\n";
    } 

    int size = sizeof(prices)/sizeof(prices[1]);
    std::cout << "The array has " << size << " elements.";

    return 0;
}