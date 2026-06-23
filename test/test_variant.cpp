#include <iostream>
#include <tue/config/variant.h>

int main(int argc, char** argv)
{

    tue::Variant v(-56);
    std::cout << v.isInt() << std::endl;
    std::cout << v.isReal() << std::endl;

    std::cout << v.toInt() << std::endl;
    std::cout << v.toReal() << std::endl;

    return 0;
}
