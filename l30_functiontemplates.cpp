# include <iostream> 

/*
A function template is a blueprint for a function that works with different data types.
The compiler generates the specific version of the function when you call it, based on the argument types (type deduction).
This lets you write one function (e.g., max) that can handle int, double, char, std::string, etc., without duplicating code.
*/

template <typename T>
T max_temps(T x, T y){
    return (x > y) ? x : y;
}

template <typename T, typename U>
auto max_both(T x, U y){
    return (x > y) ? x : y;
}

int main(){
    std::cout << max_temps(8, 10) << '\n';
    std::cout << max_temps(0.6658, 0.9812) << '\n';
    std::cout << max_temps('a', 'b') << '\n';
    std::cout << max_temps(std::string("Edward"), std::string("Tapiwa")) << '\n';
    std::cout << max_both(8, 10.6525) << '\n';
    std::cout << max_both(0.6658, 1) << '\n';
    std::cout << max_both('a', 0.2584) << '\n';
    // std::cout << max_both(9, "Tapiwa") << '\n';
    return 0;
}