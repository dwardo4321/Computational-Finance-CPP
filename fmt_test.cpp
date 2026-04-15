#include <iostream>
#include <fmt/core.h>

int main() {
    std::string name = "Edward";
    double price = 123.456789;

    std::string msg = fmt::format("Hello, {}. Price = {:.2f}", name, price);
    std::cout << msg << "\n";

    return 0;
}