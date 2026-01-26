#include <iostream>

/* Recursion in C++ is when a function calls itself to solve a problem
   by breaking it into smaller subproblems.
   Each recursive call gets its own stack frame, and the process stops when
   a base case is reached. Without a base case, recursion causes infinite calls
   and a stack overflow. Recursion is useful for problems like tree traversal,
   factorials, and divide-and-conquer algorithms. */

void walk(int steps){

     for (int i = 0; i < steps; i++){
        (i + 1) % 2 == 0 ? std::cout << "Right Step \n": std::cout << "Left Step \n";     
    }
}

long long factorial(int num){
    long long result = 1;

    for(int i = 1; i <= num; i++){
        result = result * i;
    }
    
    return result;
}

long long factorial_rec(int num){
    if(num > 1){
        return num * factorial_rec(num - 1);        
    } else {
        return 1;
    }
}

int main(){

    walk(5);
    std::cout << factorial(17) << '\n';
    std::cout << factorial_rec(16) << '\n';
    std::cout << sizeof(int) << '\n';
    std::cout << sizeof(long long) << '\n';
    return 0;
}