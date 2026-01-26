#include <iostream>
/*
  Enums (enumerations) are used to define a type with a fixed set of named integral constants,
  improving readability and safety over "magic numbers" (e.g., Direction::Left instead of 0).
 
  They are commonly used to represent states, modes, categories, options, and flags.
 
  Prefer enum class (scoped enum) to avoid name pollution and accidental implicit conversions to int:
    enum class Color { Red, Green, Blue };
*/

enum Day {sunday, monday, tuesday, wednesday, thursday, friday, saturday};


int main(){
    Day today = monday;

    switch(today){
        case sunday: std::cout << "It is sunday!";
                        break;
        case monday: std::cout << "It is monday!";
                        break;
        case tuesday: std::cout << "It is tuesday!";
                        break;
        case wednesday: std::cout << "It is wednesday!";
                        break;
        case thursday: std::cout << "It is thursday!";
                        break;
        case friday: std::cout << "It is friday!";
                        break;
        case saturday: std::cout << "It is saturday!";
                        break;
    }


    return 0;
}