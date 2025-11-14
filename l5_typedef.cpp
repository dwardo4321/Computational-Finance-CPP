#include <iostream>
#include <vector>

//typedef std::vector<std::pair<std::string, int>> pairlist_t
typedef std::string text_t;
typedef int number_t;

//More popular because its suitable when using templates
//using text_t = std::string
//using number_t = int

int main(){

    using std::cout;

    /*
    In C++, the **`typedef`** keyword is used to create an alias, or an alternative name, for an existing 
    data type to make code more readable and easier to manage. It allows you to define shorter or more 
    meaningful names for complex type declarations—for example, you can write `typedef unsigned long ulong;
    ` so that `ulong` can be used instead of repeatedly typing `unsigned long`. `typedef` is especially 
    helpful when dealing with pointers, structures, or templates, where type definitions can become long 
    and cumbersome. In modern C++, the `using` keyword serves a similar purpose and is generally preferred 
    for defining type aliases because it offers clearer syntax and works better with templates.
    */

    //pairlist_t pairlist;

    text_t firstname = "Tapiwa Edward";
    number_t age = 21;

    cout << "My name is " << firstname << ", I am " << age << " years old.";

    return 0;
}