#include <iostream>

/*
  The 'break' and 'continue' statements control loop execution flow in C++. 
  'break' immediately terminates the entire loop and moves execution to the 
  first statement after the loop—useful when a stopping condition is met early, 
  such as finding a value in a dataset or exiting a Monte Carlo simulation loop.
  'continue' skips the rest of the current iteration and jumps to the next one,
  often used to ignore invalid data points, skip unwanted states in a Markov
  chain simulation, or bypass exceptional conditions during numerical loops.
 */


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