#include <catch2/catch_test_macros.hpp>

double add(double a, double b) {
    return a + b;
}

TEST_CASE("basic arithmetic works") {
    REQUIRE(add(2.0, 3.0) == 5.0);
    REQUIRE(add(-1.0, 1.0) == 0.0);
}