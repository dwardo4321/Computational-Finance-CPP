#include <iostream>
#include <string>
#include <vector>
#include <boost/version.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

int main() {
    std::string s = "boost test works";

    boost::to_upper(s);

    int x = boost::lexical_cast<int>("123");
    std::vector<std::string> parts;
    boost::split(parts, s, boost::is_any_of(" "));

    std::cout << "Boost version macro: " << BOOST_VERSION << "\n";
    std::cout << "Boost version string: " << BOOST_LIB_VERSION << "\n";
    std::cout << "Upper string: " << s << "\n";
    std::cout << "Converted number: " << x << "\n";
    std::cout << "Split words:\n";

    for (const auto& p : parts) {
        std::cout << " - " << p << "\n";
    }

    return 0;
}