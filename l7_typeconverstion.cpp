#include <iostream>

/*
In C++, **type conversion** refers to changing a value from one data type to another, and it can 
happen in two ways: **implicit** and **explicit**. **Implicit type conversion**, also known as 
*type promotion* or *coercion*, occurs automatically when the compiler converts one data type to 
another to ensure expressions are compatible—for example, converting an `int` to a `double` in a 
mixed arithmetic operation. **Explicit type conversion**, also called *type casting*, is performed 
intentionally by the programmer using a cast operator, such as `(double)x` or `static_cast<double>(x)`, 
to control how and when the conversion happens. Explicit conversions give more precision and prevent 
unexpected behavior that can result from automatic conversions.
*/

void demo1(){
    using std::cout;
    double x = (int) 3.054;
    char y = 100;
    char z = 1000;

    cout << "We have " << (int) z << ", "<< x << " and " << y << "\n";
}

void demo2(){
    using std::cout;
    int correct = 8;
    int questions = 10;
    double score1 =  correct/questions * 100;
    double score2 =  (double) correct/questions * 100;

    cout << "They scored " << score1 << "% and " << score2 << "%.";
}

int main(){
    demo1();
    demo2();
    return 0;
}