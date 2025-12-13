#include <iostream>

/* Dynamic memory in C++ is memory you request at runtime 
   (instead of being created automatically on the stack).
   It lives on the heap and stays allocated until you free 
   it (or an owning object frees it), so you can create
   objects whose size/lifetime aren’t known at compile time. 
   In modern C++, prefer RAII types like std::vector
   and smart pointers (std::unique_ptr/std::shared_ptr) over 
   raw new/delete to avoid leaks and dangling pointers. */

int main(){

    int *pNum = NULL;
    char *pGrades = NULL;

    pNum = new int;
    pGrades = new char[5];

    std::cout << "Address: " << pNum << '\n';
    std::cout << "Value: " << pNum << '\n';

    delete pNum;
//----------------------------------------------------
    int size;

    std::cout << "How many grades to enter in?: ";
    std::cin >> size;

    pGrades = new char[size];

    for (int i = 0; i < size; i++){
        std::cout << "Enter grade #" << i + 1 << ": ";
        std::cin >> pGrades[i];
    }

    for (int i = 0; i < size; i++){
        std::cout << pGrades[i] << " ";
    }
    
    

    return 0;



}