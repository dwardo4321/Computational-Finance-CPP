#include <iostream>
#include <cmath>

int main(){
    using std::cout; 

    double x = 11.54, y = 3.8921;

    cout << std::max(x, y) << '\n';
    cout << std::min(x, y) << '\n';
    cout << pow(x, y) << '\n';
    cout << sqrt(x) << '\n';
    cout << round(x) << '\n';
    cout << ceil(y) << '\n';
    cout << floor(x) << '\n';
    return 0;
    // for more: https://cplusplus.com/reference/cmath/
}