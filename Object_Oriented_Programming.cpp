#include <iostream>
#include <string>

// Abstraction
class abstractemployee{
    // Make method obligatory
    virtual void askforpromotion()=0;
};


// Encapsulation
// Access modifiers: private (default), public, protected
class employee:abstractemployee{
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
        void askforpromotion(){
            if (Age > 30)
                std::cout << Name << " got promoted! \n";
            else 
                std::cout << Name << ", sorry, no promotion! \n";
        }
};

// Inheritance
class developer: public employee{  // employee is the base class
    // inherits the properties of employee
    public:
        std::string Favprogramminglang;
        developer(std::string name, std::string company, int age, std::string favprogramminglang)
            :employee(name, company, age){
                Favprogramminglang = favprogramminglang;
                }

        void fixbug(){
            std::cout << getName() << " is fixing bugs in " << Favprogramminglang << '\n';
        }
};

class teacher: public employee{
     public:
        std::string Subject;
        teacher(std::string name, std::string company, int age, std::string subject):
            employee(name, company, age){
                Subject = subject;
            }
        void preparelesson(){
            std::cout << getName() << " is preparing for a " <<  Subject << " lesson! \n";
        }
};


int main(){
    employee worker_1 = employee("Edward", "BoA", 32);
    employee worker_2 = employee("Tapiwa", "JPMorgan", 22);

    developer dev_1 = developer("The prophet", "Company", 25, "C++");
    teacher  teach_1 = teacher("Hazel", "LFC", 26, "Physiotherapy");
    // worker_1.name = "Edward";
    // worker_1.company = "BoA";
    // worker_1.age = 32;

    /* worker_2.name = "Tapiwa";
    worker_2.company = "JPMorgan";
    worker_2.age = 34; */

    //worker_1.intro();
    //worker_2.intro();

    //worker_1.setAge(18);
    //std::cout << worker_1.getAge();

    worker_1.askforpromotion();
    worker_2.askforpromotion();
    dev_1.fixbug();
    dev_1.askforpromotion();
    teach_1.askforpromotion();
    teach_1.preparelesson();


    return 0;
}