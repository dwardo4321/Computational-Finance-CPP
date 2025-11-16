#include <iostream>
#include <cmath>

using std::cout;
void demo1(){
    int age;
    cout << "Enter your age: ";
    std::cin >> age;

    if(age >= 20){
        cout << "Welcome to adulthood!" << "\n";
    }
    else if(18 <= age && age < 20){
        cout << "Wait a bit!" << "\n";
    }
    else if(0 < age && age < 18){
        cout << "You are too young!" << "\n";
    }
    else{
        cout << "WRONG INPUT!" << "\n";
        //cout << "Enter your age: ";   For do while loops
        //std::cin >> age;
    }
}
/*
&& -> (and) if both statements are true
|| -> (or) if one of the statements is true
!  -> (not) reverses the statement
*/



/*In C++ (cpp), a **`switch` statement** is a control structure used to execute one block of code out of several
 options based on the value of a single variable or expression. It provides a cleaner and more efficient alternative
 to using multiple `if-else` statements when comparing the same variable against different constant values.
 Each possible value is handled by a **`case`**, and when a match is found, the corresponding block runs until a
 `break` statement is encountered, which prevents execution from “falling through” to the next case. If none of 
 the cases match, an optional **`default`** block can run as a fallback. Switch statements are commonly used for 
 menu selections, command handling, and other scenarios involving discrete choices.
*/

void demo2(){
    int month;
    cout << "Enter the month (1-12): ";
    std::cin >> month;

    switch(month){
        case 1:
            cout << "Its January" << "\n";
            break;
        case 2:
            cout << "Its February" << "\n";
            break;
        case 3:
            cout << "Its March" << "\n";
            break;
        case 4:
            cout << "Its April" << "\n";
            break;
        case 5:
            cout << "Its May" << "\n";
            break;
        case 6:
            cout << "Its June" << "\n";
            break;
        case 7:
            cout << "Its July" << "\n";
            break;
        case 8:
            cout << "Its August" << "\n";
            break;
        case 9:
            cout << "Its September" << "\n";
            break;
        case 10:
            cout << "Its October" << "\n";
            break;
        case 11:
            cout << "Its November" << "\n";
            break;
        case 12:
            cout << "Its December" << "\n";
            break; 
        default:
            cout << "Please enter numbers 1 through 12!" << "\n";
    }
}

void demo3(){
    char grade;
    cout << "Enter the grade (A-F): ";
    std::cin >> grade;

    switch(grade){
        case 'A':
            cout << "You did great!!!";
            break;
        case 'B':
            cout << "You did good!!";
            break;
        case 'C':
            cout << "You did okay!";
            break;
        case 'D':
            cout << "You did not do good.";
            break;
        case 'F':
            cout << "You failed.";
            break;
        default:
            cout << "Please enter letters A-F only!";
    }
}

int main(){
    demo1();
    demo2();
    demo3();
    return 0;
}