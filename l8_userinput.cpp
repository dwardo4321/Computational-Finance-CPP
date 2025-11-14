#include <iostream>

int main(){
    using std::cout;

    std::string name;
    int age;

    cout << "What is your full name, and how old are you? " << "\n";
    std::getline(std::cin >> std::ws, name); //For when we have spaces in the string
    std::cin >> age;
    cout << "Your name is " << name << ", and you're " << age << " years old.";

    return 0;
}