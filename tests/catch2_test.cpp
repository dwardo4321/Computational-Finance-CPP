#include <catch2/catch_test_macros.hpp>

double add(double a, double b) {
    return a + b;
}

TEST_CASE("basic arithmetic works") {
    REQUIRE(add(2.0, 3.0) == 5.0);
    REQUIRE(add(-1.0, 1.0) == 0.0);
}

/* 
C:/msys64/ucrt64/bin/g++.exe -std=c++20 -g catch2_test.cpp -o catch2_test.exe -lCatch2Main -lCatch2
.\catch2_test.exe 
*/
