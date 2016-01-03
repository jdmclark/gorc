#include "print_message.hpp"
#include <iostream>

void print_message(message const &msg)
{
    std::cout << msg.str() << std::endl;
}
