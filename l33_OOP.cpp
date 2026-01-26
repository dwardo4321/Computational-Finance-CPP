#include <iostream>

/*
 OOP (Object-Oriented Programming) in C++ is a way of designing programs around "objects"—instances of classes/structs
 that bundle data (state) and functions (behavior) together.

 Core ideas:
  - Encapsulation: hide internal details and expose a clear public interface (private/protected/public).
  - Abstraction: model only what matters; provide simple interfaces and hide complexity.
  - Inheritance: build new types from existing ones to reuse/extend behavior (is-a relationship).
  - Polymorphism: treat different derived types through a common base interface (often via virtual functions),
                  enabling dynamic dispatch at runtime; also includes compile-time polymorphism via templates/overloads.

 Why use it:
  - Improves modularity, reuse, maintainability, and makes large systems easier to reason about.
*/

//Class: blue print to create objects
class Human{

    // class attributes
    public:
        std::string name;
        std::string occupation;
        int age;

    // methods (function belonging to a class, things an object can perform)
    void eat(){
        std::cout << "This person is eating \n";
    }
    void drink(){
        std::cout << "This person is drinking \n";
    }
    void sleep(){
        std::cout << "This person is sleeping \n";
    }
    void study(){
        std::cout << "This person is studying \n";
    }
};


int main(){

    Human human_1, human_2;
    human_1.name = "Edward";
    human_1.occupation = "Quantitative Developer";
    human_1.age = 31;

    human_2.name = "Tapiwa";
    human_2.occupation = "Quantitative Trader";
    human_2.age = 32;

    std::cout << human_1.name << '\n';
    std::cout << human_1.occupation << '\n';
    std::cout << human_1.age << '\n';
    std::cout << human_2.name << '\n';
    std::cout << human_2.occupation << '\n';
    std::cout << human_2.age << '\n';

    human_1.eat();
    human_1.study();
    human_2.sleep();
    human_2.drink();
    
    return 0;
}