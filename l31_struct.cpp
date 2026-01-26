# include <iostream> 

/*
  A struct in C++ is a user-defined type that groups related data (and optionally functions) into a single object.
  It is essentially the same as a class, except members and inheritance are public by default (instead of private).
*/

struct students{
    std::string name;
    double gpa;
    bool enrolled;
};
    
struct cars{
    std::string model;
    int year;
    std::string color;
};

void printCar(cars car){
    std::cout << "------------------------------------------- \n";
    std::cout << car.model << '\n' << car.year << '\n' << car.color << '\n';
    std::cout << "------------------------------------------- \n";
}

void paintCar(cars &car, std::string colour){
    car.color = colour;
}



int main(){
    students student_1;
    student_1.name = "Edward";
    student_1.gpa = 4.0;
    student_1.enrolled = true;

    std::cout << student_1.name << '\n' << student_1.gpa << '\n' << student_1.enrolled << '\n';
    std::cout << "------------------------------------------- \n";

    students student_2;
    student_2.name = "Tapiwa";
    student_2.gpa = 3.9;
    student_2.enrolled = true;

    std::cout << student_2.name << '\n' << student_2.gpa << '\n' << student_2.enrolled << '\n';
    std::cout << "------------------------------------------- \n";

    students student_3;
    student_3.name = "Idiot";
    student_3.gpa = 2.9;
    student_3.enrolled = false;

    std::cout << student_3.name << '\n' << student_3.gpa << '\n' << student_3.enrolled << '\n';
    std::cout << "------------------------------------------- \n";

    cars car1, car2;
    car1.model = "Benz";
    car1.year = 2025;
    car1.color = "navy";

    car2.model = "BMW";
    car2.year = 2026;
    car2.color = "silver";

    paintCar(car1, "white");
    paintCar(car2, "black");

    printCar(car1);
    printCar(car2);

    return 0;
}