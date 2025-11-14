#include <iostream>
int main(){

    //integer (whole number)
    int x = 5; //declaration
    int y = 6; //declaration
    int sum = x + y; //declaration
    int days = 7.5;
 
    std::cout << x << '\n';
    std::cout << y << '\n';
    std::cout << sum << '\n';
    std::cout << days << '\n';  

    //double (number including a decimal) -------------------------------------
    double age = 21;
    double year = 2023;
    double price = 10.99;

    std::cout << "-----------" << '\n';
    std::cout << age << '\n';
    std::cout << year << '\n';
    std::cout << price << '\n';

    // single character (only stores a single character) -------------------------------------
    char grade = 'A';
    char initial = 'ABC';
    //char dollar = "$";

    std::cout << "-----------" << '\n';
    std::cout << grade << '\n';
    std::cout << initial << '\n';
    //std::cout << dollar << '\n';

    // boolean (T or F) -------------------------------------
    bool tryout = true;
    bool pass = false;
    
    std::cout << "-----------" << '\n';
    std::cout << tryout << '\n';
    std::cout << pass << '\n';

     // string (sequence of text) -------------------------------------  
    std::cout << "-----------" << '\n';
    std::string name = "Taps";
    std::string day = "Monday";
    std::string food = "pizza";
    std::string address = "9 Chatsworth Rd";

    std::cout << "Hello " << name << "\n";
    std::cout << "You are " << age << "years old!";
    return 0;
}