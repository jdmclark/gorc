#include "io/std_input_stream.hpp"
#include "io/std_output_stream.hpp"
#include <stdexcept>
#include <system_error>
#include <vector>
#include <iostream>
#include <cstring>
#include <iomanip>

int main(int, char**)
{
    gorc::std_input_stream is;
    is.reopen_as_binary();

    // Read a string from stdin containing CRLF. On Windows in text mode
    // the CRLF will be replaced with LF.
    char in_str[14];
    is.read(in_str, 14);
    std::string in_msg(in_str, 14);

    std::cout << "Input:";
    for(char ch : in_msg) {
        std::cout << "  " << (std::isspace(ch) ? '?' : ch);
    }
    std::cout << std::endl;

    std::cout << "Input:";
    for(char ch : in_msg) {
        std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
    }
    std::cout << std::endl;

    std::cout << "Output: ";

    // Write a string containing LF. On Windows in text mode the LF will be transformed
    // into a CRLF.
    std::string out_msg = "Message\nWith\nNewlines\n";

    gorc::std_output_stream os;
    os.reopen_as_binary();

    os.write(out_msg.c_str(), out_msg.size());

    return 0;
}
