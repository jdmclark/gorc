#include "io/std_output_stream.hpp"
#include <vector>
#include <iostream>
#include <cstring>

int main(int, char**)
{
    gorc::std_output_stream os;

    std::string str = "Hello, World!\n";
    os.write(str.data(), str.size());

    return 0;
}
