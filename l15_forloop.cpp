#include <iostream>

void demo1(){
    for (int i = 1; i <= 10; i++){
        std::cout << i << ": I love God!" << std::endl;
    }
}

void demo2(){
    for (int i = 1; i <= 10; i+=2){
        std::cout << i << ": Praise God!" << std::endl;
    }
}

void demo3(){
    for (int i = 10; i >= 1; i--){
        std::cout << i << ": Worship God!" << std::endl;
    }
}

void demo4(){
    for (int i = 10; i >= 1; i-=2){
        std::cout << i << ": Grace and Mercy!" << std::endl;
    }
}

int main(){
    demo1();
    demo2();
    demo3();
    demo4();
    return 0;
}