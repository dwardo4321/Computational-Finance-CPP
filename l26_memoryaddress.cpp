#include <iostream>

// Pass by address

void swap(std::string &x, std::string &y, std::string &z){
    std::string temp1;
    temp1 = x;
    x = y; 
    y = temp1;
    z.append(temp1);
}

int main(){

    std::string name = "Edward";
    int age = 31;
    bool student = true;

    std::cout << &name << '\n';
    std::cout << &age << '\n';
    std::cout << &student << '\n';

    std::string x = "123";
    std::string y = "OneTwoThree";
    std::string z = "combo";

    swap(x, y, z);

    std::cout << "x = " << x << '\n'; 
    std::cout << "y = " << y << '\n';
    std::cout << "z = " << z << '\n';

    return 0;
}