#include <iostream>

void demo1(){
    using std::cout;

    int students = 100;
    //ADDING
    //students = students + 5;
    //OR
    students += 5;
    students ++; //Or you only want to add one student....106 students

    //SUBTRACTING
    //students = students - 5;
    //OR
    students -= 50;
    students --; //Or you only want to remove one student....55 students

    //MULTIPLYING
    //students = students * 3;
    students *= 3;//....165

    //DIVIDING
    //students = students / 2;
    students /= 2;//....82 (integer)

    //REMAINDER
    int students_sat;
    students_sat = students / 5;
    students %= 5;//....2 (integer)

    cout << "Students who sat for the exam: " << students_sat << " and the remainder " << students;
}

main(){
    demo1();

    return 0;
}