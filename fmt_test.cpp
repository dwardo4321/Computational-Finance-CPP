#include <iostream>
#include <fmt/core.h>

int main() {
    std::string name = "Edward";
    double price = 123.456789;

    std::string msg = fmt::format("Hello, {}. Price = {:.2f}", name, price);
    std::cout << msg << "\n";

    return 0;
}

/*
C:/msys64/ucrt64/bin/g++.exe -std=c++20 -g fmt_test.cpp -o fmt_test.exe -lfmt
.\fmt_test.exe
*/