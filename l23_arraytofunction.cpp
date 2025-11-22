#include <iostream>
#include <random>
#include <ctime>
#include <iomanip>

std::mt19937 gen(static_cast<unsigned>(std::time(nullptr)));

double getTotal(double prices[], int size){
    
    double total = 0;
    for (int i = 0; i < size; i++){
        total += prices[i];
    }
    return total;
}

int main(){

    constexpr int n = 100000;
    double prices[n] = {};
    double num;

    for (int i = 0; i < n; ++i)
    {
        num = gen() / (static_cast<double>(gen.max()) + 1);
        prices[i] = num;
    } 

    int size = sizeof(prices)/sizeof(prices[1]);

    double total = getTotal(prices, size);
    std::cout << "The total price is $"<< std::setprecision(2) << std::fixed << total * 111;
    return 0;
}