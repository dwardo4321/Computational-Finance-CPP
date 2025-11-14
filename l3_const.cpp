#include <iostream>
int main(){
    const double pi = 22/7; //variable cant be changed now
    //double pi = 13.4641;
    double radius = 2.5;
    double circumference = 2 * pi * radius;

    std::cout << "Our circumference is " << circumference << "cm";

    return 0;
}