#include <iostream>
#include <cstdlib>
#include <boost/filesystem.hpp>

int main(int, char **)
{
    if(boost::filesystem::is_regular_file("testproject.json")) {
        std::cout << "test.boc exists" << std::endl;
    }
    else {
        std::cout << "test.boc does not exist" << std::endl;
    }

    return EXIT_SUCCESS;
}
