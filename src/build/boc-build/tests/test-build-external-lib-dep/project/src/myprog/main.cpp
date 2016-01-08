#include <iostream>
#include <cstdlib>
#include "mylib/does_test_boc_exist.hpp"

int main(int, char **)
{
    if(does_test_boc_exist()) {
        std::cout << "test.boc exists" << std::endl;
    }
    else {
        std::cout << "test.boc does not exist" << std::endl;
    }

    return EXIT_SUCCESS;
}
