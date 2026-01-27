#include <iostream>


// Access modifiers: private (default), public, protected
class employee{
    private:
        std::string Name;
        std::string Company;
        int Age;

    public:
        // Method ****************************************
        void intro(){
            std::cout << "Name - " << Name << '\n';
            std::cout << "Company - " << Company << '\n';
            std::cout << "Age - " << Age << '\n';
        }
        //---------------------------------------------------
        void setName(std::string name){//setter
            Name = name;
        }
        std::string getName(){//getter
            return Name;
        }
        //---------------------------------------------------
        void setCompany(std::string company){//setter
            Company = company;
        }
        std::string getCompany(){//getter
            return Company;
        }
        //---------------------------------------------------
        void setAge(int age){//setter
            if(age >= 18)
            Age = age;
        }
        int getAge(){//getter
            return Age;
        }
        
        // Constructor ****************************************
        employee(std::string name, std::string company, int age){
            Name = name;
            Company = company;
            Age = age;
        }
};

int main(){
    employee worker_1 = employee("Edward", "BoA", 32);
    employee worker_2 = employee("Tapiwa", "JPMorgan", 34);
    // worker_1.name = "Edward";
    // worker_1.company = "BoA";
    // worker_1.age = 32;

    /* worker_2.name = "Tapiwa";
    worker_2.company = "JPMorgan";
    worker_2.age = 34; */

    //worker_1.intro();
    //worker_2.intro();

    worker_1.setAge(18);
    std::cout << worker_1.getAge();

    return 0;
}