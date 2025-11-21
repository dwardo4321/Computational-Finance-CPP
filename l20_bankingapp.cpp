#include <iostream>
#include <iomanip>

void showBalance(double balance){
    std::cout << "------------------------------------\n";
    std::cout << "Your balance is $" << std::setprecision(2) << std::fixed << balance << "\n";
    std::cout << "------------------------------------\n";
}

double deposit(){
    double deposited;
    std::cout << "Enter amount do be deposited: $";
    std::cin >> deposited;
    if (deposited < 0)
    {
        std::cout << "------------------------------------\n";
        std::cout << "Invalid figure!!!!" << "\n";
        return 0;
    }
    
    return deposited;
}

double withdraw(double balance){
    double withdrawal;
    do{
    std::cout << "Enter amount do be withdrawn: $";
    std::cin >> withdrawal;
    if (withdrawal < 0)
    {
        std::cout << "------------------------------------\n";
        std::cout << "Invalid figure!!!!" << "\n";
        return 0;
    } else if (withdrawal > balance)
    {
        std::cout << "------------------------------------\n";
        std::cout << "Insufficient funds!!!!" << "\n";
        return 0;
    }
    return withdrawal;} while (withdrawal < 0);
}

int main(){
    
    double balance = 100000; 
    int choice = 0;

    do{ 
        std::cout << "1. Show balance\n";
        std::cout << "2. Deposit money\n";
        std::cout << "3. Withdraw money\n";
        std::cout << "4. Exit\n";
        std::cout << "*******************************\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        
        switch (choice)
        {
            case 1: showBalance(balance);
                break;
            case 2: balance += deposit();
                    showBalance(balance);
                break;
            case 3: balance -= withdraw(balance);
                    showBalance(balance);
                break;
            case 4: std::cout << "------------------------------------\n";
                    std::cout << "Thank you for visiting!";
                break;  
            default:std::cout << "------------------------------------\n"; 
                    std::cout << "Invalid choice. \n";}
                    std::cout << "------------------------------------\n";        
    } while(choice != 4);
    return 0;
}