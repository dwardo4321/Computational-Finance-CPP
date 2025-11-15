#include <iostream>

using std::cout;

void demo1(){
    cout << "Enter 1st number: ";
    int lets_try; 
    std::cin >> lets_try;
    7 > lets_try ? cout << "Good, Next!" << "\n" : cout << "Try Again" << "\n";
}

void demo2(){
    cout << "Enter number: ";
    int num;
    std::cin >> num;
    num % 2 == 0 ? cout << "EVEN" << "\n": cout << "ODD" << "\n";
}

void demo3(){
    bool smart, pass = true;
    smart ? cout << "Knew it." << "\n": cout << "Lets find something to eat" << "\n";
    cout << (pass ? "Lets go!!!": "Meeehhh!!!");
}

int main(){
    demo1();
    demo2();
    demo3();
    return 0;
}