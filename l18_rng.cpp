#include <iostream>
#include <cmath>
#include <ctime>
#include <vector>


std::vector<double> demo1(){
    std::vector<double> rng;
    double num;
    int i;
    for(i = 0; i <= 1000; i++){
        do{
            int sign = (rand() % 2) * 2 - 1;  
            num = sign * exp(rand() % 11);
        }
        while(num <= -50000 || num >= 50000);
        rng.push_back(num);
    }
    return rng;
}

int main(){
    srand(time(nullptr));
    std::vector<double> result = demo1();   // <-- Store it

    // Print it
    for (double x : result) {
        std::cout << x << std::endl;
    }
    return 0;
}