#include <iostream>
#include <cmath>
using std::cout;
void demo1(){
    double num1, num2;
    char Operator_In;

    cout << "Enter first number:" << "\n";
    std::cin >> num1;
    cout << "Operator_In (/, *, +, -, ^):" << "\n";
    std::cin >> Operator_In;
    cout << "Enter second number:" << "\n";
    std::cin >> num2;

    switch (Operator_In){
        case '/':
            cout << num1 / num2;
        break;
        case '*':
            cout << num1 * num2;
        break;
        case '+':
            cout << num1 + num2;
        break;
        case '-':
            cout << num1 - num2;
        break;
        case '^':
            cout << pow(num1, num2);
        break;
    
    default:
        break;
    }   



}

int main(){
    demo1();
    return 0;
}