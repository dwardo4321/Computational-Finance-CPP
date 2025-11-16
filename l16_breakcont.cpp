#include <iostream>

int i;

void demo1(){
    for(i = 1; i <= 20; i++){
        if(i == 15){
            break;
        }
        std::cout << i << '\n';
    }
    std::cout << "--------" << '\n';
}

void demo2(){
    for(i = 1; i <= 20; i++){
        if(i == 15){
            continue;
        }
        std::cout << i << '\n';
    }
}

int main(){
    demo1();
    demo2();
    return 0;
}