#include <iostream>
#include <random>
#include <ctime>

/*
A *for-each* loop (also called a range-based for loop in C++) is a loop that iterates
directly over the elements of a container, rather than over their indices. For example, 
`for (int x : vec)` visits each value in `vec` one by one, without you writing `vec[i]` 
or managing `i`. In contrast, a classic `for` loop like `for (int i = 0; i < vec.size(); ++i)` 
uses an index variable, and you access elements with `vec[i]`. That means with a normal `for` 
loop you always have the index available (useful if you need both position and value), while 
a for-each loop hides the index and just gives you the element, which is simpler and less 
error-prone when you don’t care about positions. Another difference is safety: with a for-each 
loop you can’t accidentally go out of bounds with a wrong condition, and it automatically adjusts 
if the container size changes (e.g. if you change the vector size in one place and forget to 
update the loop limit).
*/

std::mt19937 gen(static_cast<unsigned>(std::time(nullptr)));

int main(){

    double prices[14] = {};
    double num;

    for (int price : prices)
    {
        num = gen() / (static_cast<double>(gen.max()) + 1);
        prices[price] = num;
        std::cout << "==> " << prices[price] << "\n";
    } 
    return 0;
}