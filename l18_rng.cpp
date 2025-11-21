#include <iostream>
#include <cmath>
#include <ctime>
#include <vector>
#include <random>

std::mt19937_64 gen(static_cast<unsigned>(std::time(nullptr)));

std::vector<double> demo1(){
    std::vector<double> rng;
    double num;
    int i;
    for(i = 1; i <= 1000; i++){
        do{
            int sign = (gen() % 2) * 2 - 1;  
            num = sign * exp(gen() % 11);
        }
        while(num <= -50000 || num >= 50000);
        rng.push_back(num);
    }
    return rng;
//int a = 5;
//int b = 15;
//unsigned int raw = gen();
//int x = a + (raw % (b - a + 1));  // x in [5, 15]
}

int main(){
    std::vector<double> result = demo1();   // <-- Store it

    // Print it
    for (double x : result) {
        std::cout << x << std::endl;
    }
    return 0;
}