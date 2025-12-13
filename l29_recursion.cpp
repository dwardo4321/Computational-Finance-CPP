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

int main(){

    walk(10);
    

    return 0;



}