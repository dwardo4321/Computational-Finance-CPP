#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    json j;

    j["model"] = "Black-Scholes";
    j["rate"] = 0.03;
    j["sigma"] = 0.20;
    j["spots"] = {100.0, 105.0, 110.0};

    std::cout << j.dump(4) << "\n";

    double rate = j["rate"];
    std::cout << "rate = " << rate << "\n";

    return 0;
}