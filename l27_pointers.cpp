#include <iostream>
#include <Eigen/Dense>

int main(){

    std::vector<double> exvector = {0.12, 0.50, 0.87, 0.95, 1.52};
    std::string name = "Edward";
    int age = 31;
    int numbers[5] = {12, 20, 30, 81, 10};

    std::vector<double> *PExvector = &exvector;
    // pExvector is a pointer that stores the memory address of exvector
    std::string *pName = &name;
    int *pAge = &age;
    int *pNumbers = numbers;

    for(double i: *PExvector)
        std::cout << i << '\n';
/* *pExvector DEREFERENCES the pointer,
    meaning: "go to the address stored in pExvector
    and access the actual vector object there" */

    std::cout << *pName << '\n';
    std::cout << *pAge << '\n';
    std::cout << *pNumbers << '\n';

    // NUL POINTER
    int *pointer1 = nullptr;
    int x = 123;

    pointer1 = &x;
    pointer1 == nullptr ? std::cout << "Address was not assigned! \n": std::cout << "Address was assigned! \n";
    
    int *pointer2 = nullptr;
    int y = 321;

    //pointer2 = &y;
    pointer2 == nullptr ? std::cout << "Address was not assigned! \n": std::cout << "Address was assigned! \n";
  
    return 0;
}