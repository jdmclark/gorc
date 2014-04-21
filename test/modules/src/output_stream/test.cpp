#include "base/io/output_stream.h"
#include <vector>
#include <iostream>
#include <cstring>

int main(int, char**) {
    gorc::io::std_output_stream os;

    std::string str = "Hello, World!\n";
    os.write(str.data(), str.size());

    return 0;
}
