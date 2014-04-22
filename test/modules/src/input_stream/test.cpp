#include "base/io/input_stream.h"
#include "base/io/exception.h"
#include <vector>
#include <iostream>
#include <cstring>

int main(int, char**) {
    gorc::io::std_input_stream is;

    std::vector<char> str;
    str.resize(14, '\0');
    is.read(str.data(), 13);

    if(strcmp(str.data(), "Hello, World!")) {
        std::cout << "Strings do not match" << std::endl;
        return 1;
    }
    else {
        std::cout << "Strings match" << std::endl;
    }

    try {
        char ch;
        is.read(&ch, 1);
        std::cout << "Exception not thrown" << std::endl;
    }
    catch(const gorc::io::file_read_exception&) {
        std::cout << "File read exception thrown" << std::endl;
    }
    catch(...) {
        std::cout << "Unknown exception thrown" << std::endl;
    }

    if(!is.at_end()) {
        std::cout << "Stream not at end" << std::endl;
        return 1;
    }
    else {
        std::cout << "Stream at end" << std::endl;
    }

    return 0;
}
