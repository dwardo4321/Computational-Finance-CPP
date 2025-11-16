#include <iostream>
using std::cout;

/*
 So getline() is used when you want a full sentence, whole
 line, or anything with spaces...
*/

void whileloop(){
std::string name;
    
    while(name.empty()){
        cout << "Enter her name: ";
        std::getline(std::cin, name);
    }
    std::cout << "She's special isnt she?" << std::endl;
}

void dowhileloop(){
    double num;

    do{
        cout << "Enter number: ";
        std::cin >> num;
    }
    while(num <= 0);
    cout << "Great";
}

/*
void infiniteloop(){
std::string name;
    
    while(1 == 1){
        cout << "HELP!!!";
    }
    std::cout << "YES!!!";
}
*/

int main(){
    whileloop();
    dowhileloop();
    //infiniteloop();
    return 0;
}