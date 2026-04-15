#include <iostream>
#include <ql/quantlib.hpp>

int main() {
    using namespace QuantLib;

    Date today(15, April, 2026);
    Calendar cal = TARGET();

    Date maturity = cal.advance(today, 6, Months);
    DayCounter dc = Actual365Fixed();

    double r = 0.05;
    Time T = dc.yearFraction(today, maturity);
    double discount = std::exp(-r * T);

    std::cout << "Today: " << today << "\n";
    std::cout << "Maturity: " << maturity << "\n";
    std::cout << "Year fraction: " << T << "\n";
    std::cout << "Discount factor: " << discount << "\n";

    return 0;
}