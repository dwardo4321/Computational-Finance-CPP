#include <iostream>
#include <random>
#include <ctime>

std::mt19937 gen(static_cast<unsigned>(std::time(nullptr)));

/* int rndm = a + (gen() % (b - a + 1))
min = a
max = b;
*/

void demo1(){
    double rndm =  0 + (gen() % (10 - 0 + 1));
    std::cout << "Here is a random number, " << rndm << "\n";
    std::cout << "Its in the range 1 to 10." << "\n";
    std::cout << "*-*-*-*-*-*-Now its your time to guess*-*-*-*-*-*-" << "\n";
}

void demo2(){
    int guess, rnd, tries;
    rnd =  1 + (gen() % (10 - 1 + 1));
    do{
        std::cout << "Choose number of tries: ";
        std::cin >> tries;
    } while (tries < 1 || tries > 5);
    

    do{
        std::cout << "Guess a number: ";
        std::cin >> guess;
        tries --;

        if (tries == 0){
            rnd == guess? std::cout << "You won on the last trial, NICE":
            std::cout << "You Lost, the correct answer was " << rnd << '\n';
            break;
        }
            else if (guess == rnd)
        {
            std::cout << "You Won!!! :" << "\n";
        }
    } while (rnd != guess);
}

int main(){
    demo1();
    demo2();    
}