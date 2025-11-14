#include <iostream>

/*
Namespace:
In C++, a namespace is used to organize and group related identifiers—such as variables, functions, classes,
 and objects—under a unique name to prevent naming conflicts. When multiple parts of a program or different 
 libraries use the same identifier names, namespaces help distinguish between them by providing a separate 
 “scope” or naming region. For example, the standard library places its functions and objects inside the std 
 namespace (like std::cout or std::vector) to avoid clashing with names defined by users. In short, namespaces 
 make code more modular, readable, and safe from accidental name collisions.
*/

namespace first{
    int x = 101;
}

namespace second{
    int x = 3;
}


void demo1(){   //----------------------------------------------------------------------------------------------

    int x = 0; //If I don't say which namespace I'm using, then the local version of the variable will be used.
    
    std::cout << "Demo1---------" << '\n';
    std::cout << first::x + second::x << '\n';
    std::cout << second::x + first::x * x << '\n';
}

void demo2(){   //----------------------------------------------------------------------------------------------
    using namespace second;
    //int x = 0;
    /*
    If I define a new variable x (above) within the namespace, even after defining using namespace second,
    then the defined variable here will take precedence.
    */
    
    std::cout << "Demo2---------" << '\n';
    std::cout << x + x << '\n';
}

void demo3(){   //----------------------------------------------------------------------------------------------
    using std::cout;
    using namespace first;
    
    cout << "Demo3---------" << '\n';
    cout << "We can avoid using std::cout all the time." << '\n';
    cout << x + x;
}

int main(){
    demo1();
    demo2();
    demo3();
    return 0;
}
