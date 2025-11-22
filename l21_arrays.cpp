#include <iostream>
#include <random>
#include <ctime>

std::mt19937 gen(static_cast<unsigned>(std::time(nullptr)));

int main(){

    double prices[14] = {};
    double num;

    for (int i = 0; i < 14; i++)
    {
        num = gen() / (static_cast<double>(gen.max()) + 1);
        prices[i] = num;
    }
    // print to check
    for (int i = 0; i < 14; ++i) {
        std::cout << "prices[" << i << "] = " << prices[i] << "\n";
    }  
    
    std::string name = "Edward";
    char grade = 'A';
    double age_now = 31.5;
    int age = 31;

    std::cout << "-----------Data Type Sizes-----------\n";
    std::cout << "The size of a string: " << sizeof(name) << '\n';
    std::cout << "The size of a character: " << sizeof(grade) << '\n';
    std::cout << "The size of a double: " << sizeof(age_now) << '\n';
    std::cout << "The size of an integer: " << sizeof(age) << '\n';
    
    // ARAYS --------------------------------------------------------

    std::string name_array[] = {"Tapiwa", "Edward", "Chida"};
    char grade_array[] = {'A','B','C','D','E','F','G','H','I','J','K','L'};
    //double age_now_array[] = {31.51, 25.25, 26.65, 23.95, 26.75, 24.05};
    int age_array[] = {31, 32, 33, 34, 35, 36};

    std::cout << "-----------Data Type Array Sizes-----------\n";
    std::cout << "The size of a string array: " << sizeof(name_array) << '\n';
    std::cout << "The size of a character array: " << sizeof(grade_array) << '\n';
    std::cout << "The size of a double array: " << sizeof(prices) << '\n';
    std::cout << "The size of an integer array: " << sizeof(age_array) << '\n';

    std::cout << "-----------Determine Array Sizes-----------\n";
    std::cout << "Len of array / no of elements)=" << sizeof(name_array)/sizeof(std::string) << '\n';
    std::cout << "Len of array / no of elements)=" << sizeof(grade_array)/sizeof(char) << '\n';
    std::cout << "Len of array / no of elements)=" << sizeof(prices)/sizeof(double) << '\n';
    std::cout << "Len of array / no of elements)=" << sizeof(age_array)/sizeof(int) << '\n';

    return 0;
}