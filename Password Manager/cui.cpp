#include <iostream> 


class gui
{
public:

    void name() {
        std::cout << "Please enter your name: " << '\n';
        std::string name;
        std::cin >> name;

        std::cout << "Hello there " << name << ", hope all is well select one of the choices below: " << '\n';
    }


};
