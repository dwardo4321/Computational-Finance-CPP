#include <iostream>
#include <cmath>
int main(){
    using std::cout;

    int a, b;
    
    cout << "Enter the first length: ";
    std::cin >> a;
    cout << "Enter the second length: ";
    std::cin >> b;

    double c = sqrt(pow(a, 2) + pow(b, 2));
    cout << "The hypotenuse is of length " << c << "!";

    return 0;
}