#include <iostream>
#include <string>

void demo1(){
    for(int i = 1; i <= 20; i++){  //columns
            for(int j = 1; j <= 20; j++){  //rows
            std::cout << j << " ";
            }
        std::cout << std::endl;
    }
}

void demo2(){
    int rows, columns;
    std::string letter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::cout << "Enter number of rows: ";
    std::cin >> rows;
    
    do{
    std::cout << "Enter number of columns: ";
    std::cin >> columns;
    }
    while(columns > 27);

    for(int i = 1; i <= rows; i++){  //rows
            for(int j = 0; j <= columns-1; j++){  //columns
            std::cout << letter[j] << " ";
            }
        std::cout << std::endl;
    }
}

int main(){
    demo1();
    demo2();
    return 0;
}