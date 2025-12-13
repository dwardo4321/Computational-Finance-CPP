#include <iostream>
#include <random>
#include <cmath>

int main(){

    std::string cars[3][10] = {{"Mustang", "Mazda", "Civic", "Corvette", "Camry", "Charger", "Wrangler", "Model S", "911", "Accord"},
                              {"Golf", "A4", "X5", "Impreza", "Supra", "Altima", "Sentra", "Jetta", "Passat", "Beetle"},
                              {"F-150", "Rav4", "Explorer", "Highlander", "Bronco", "Tacoma", "Tundra", "Silverado", "Colorado", "Ranger"}};

    std::cout << cars[0][0] << '\n';
    std::cout << cars[0][1] << '\n';
    std::cout << cars[1][9] << '\n';
    std::cout << cars[2][1] << '\n';
    std::cout << cars[1][0] << '\n';
    std::cout << cars[2][0] << '\n';

    //std::cout << 
    
    int rows = sizeof(cars)/sizeof(cars[0]);
    int columns = sizeof(cars[0])/sizeof(cars[0][0]);

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            std::cout << cars[i][j] << " ";
        }
        std::cout << "\n";
    };
    return 0;
}
