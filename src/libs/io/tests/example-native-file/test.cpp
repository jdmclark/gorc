#include "io/native_file.hpp"
#include <stdexcept>
#include <system_error>
#include <vector>
#include <iostream>
#include <cstring>

int main(int, char**)
{
    std::cout << "==== Simple create/read ====" << std::endl;
    try {
        std::string str = "Hello, World!";

        {
            auto g = gorc::make_native_file("tempdir/simple.txt");
            g->write(str.data(), str.size());
        }

        auto f = gorc::make_native_read_only_file("tempdir/simple.txt");
        std::cout << "---- Contents ----" << std::endl;

        std::vector<char> str2;
        str2.resize(13);
        f->read(str2.data(), 13);
        str2.push_back('\0');

        if(!strcmp(str.data(), str2.data())) {
            std::cout << "str and str2 match" << std::endl;
        }

        if(f->at_end()) {
            std::cout << "f at end" << std::endl;
        }

        f->seek(-13);

        std::vector<char> str3;
        str3.resize(13);
        f->read(str3.data(), 13);
        str3.push_back('\0');

        if(!strcmp(str.data(), str3.data())) {
            std::cout << "str and str3 match" << std::endl;
        }

        if(f->at_end()) {
            std::cout << "f at end" << std::endl;
        }

        f->set_position(0);

        std::vector<char> str4;
        str4.resize(13);
        f->read(str4.data(), 13);
        str4.push_back('\0');

        if(!strcmp(str.data(), str4.data())) {
            std::cout << "str and str4 match" << std::endl;
        }

        if(f->at_end()) {
            std::cout << "f at end" << std::endl;
        }

        try {
            char c;
            f->read(&c, sizeof(char));
            std::cout << "exception not thrown" << std::endl;
        }
        catch(std::runtime_error const &e) {
            std::cout << e.what() << std::endl;
            std::cout << "file read exception thrown" << std::endl;
        }
        catch(...) {
            std::cout << "unknown exception thrown" << std::endl;
        }

        if(f->at_end()) {
            std::cout << "f at end" << std::endl;
        }
    }
    catch(...) {
        std::cout << "Exception thrown" << std::endl;
    }

    std::cout << "==== Overwrite existing ====" << std::endl;
    try {
        auto f = gorc::make_native_file("tempdir/existing.txt");

        std::string str = "Contents overwritten\n";
        f->write(str.data(), str.size());
    }
    catch(...) {
        std::cout << "Exception thrown" << std::endl;
    }

    std::cout << "==== Truncate failure ====" << std::endl;
    try {
        auto f = gorc::make_native_file("tempdir/write_denied.txt");
        std::cout << "Exception not thrown" << std::endl;
    }
    catch(std::system_error const &e) {
        // Check that the error message matches the system code.
        if(std::system_category().message(static_cast<int>(std::errc::permission_denied)) ==
           e.what()) {
            std::cout << "Error message matches expected" << std::endl;
        }
        else {
            std::cout << "Error messages do not match" << std::endl;
        }
        std::cout << "File create exception thrown" << std::endl;
    }
    catch(...) {
        std::cout << "Unknown exception thrown" << std::endl;
    }

    std::cout << "==== Open failure ====" << std::endl;
    try {
        auto f = gorc::make_native_read_only_file("tempdir/read_denied.txt");
        std::cout << "Exception not thrown" << std::endl;
    }
    catch(std::system_error const &e) {
        // Check that the error message matches the system code.
        if(std::system_category().message(static_cast<int>(std::errc::no_such_file_or_directory)) ==
           e.what()) {
            std::cout << "Error message matches expected" << std::endl;
        }
        else {
            std::cout << "Error messages do not match" << std::endl;
        }
        std::cout << "File open exception thrown" << std::endl;
    }
    catch(...) {
        std::cout << "Unknown exception thrown" << std::endl;
    }

    std::cout << "==== Read failure ====" << std::endl;
    try {
        auto f = gorc::make_native_read_only_file("tempdir/small.txt");
        std::vector<char> data;
        data.resize(1024);
        f->read(data.data(), 1024);
        std::cout << "No exception thrown" << std::endl;
    }
    catch(std::runtime_error const &e) {
        std::cout << e.what() << std::endl;
        std::cout << "File read exception thrown" << std::endl;
    }
    catch(...) {
        std::cout << "Unknown exception thrown" << std::endl;
    }

    std::cout << "==== Seek failure ====" << std::endl;
    try {
        auto f = gorc::make_native_read_only_file("tempdir/small.txt");
        f->seek(-1024);
        std::cout << "No exception thrown" << std::endl;
    }
    catch(std::runtime_error const &e) {
        std::cout << e.what() << std::endl;
        std::cout << "File seek exception thrown" << std::endl;
    }
    catch(...) {
        std::cout << "Unknown exception thrown" << std::endl;
    }

    std::cout << "==== Position ====" << std::endl;
    try {
        auto f = gorc::make_native_read_only_file("tempdir/small.txt");

        std::cout << "Current position: " << f->position() << std::endl;

        f->set_position(3);

        std::cout << "Current position: " << f->position() << std::endl;

        f->seek(2);

        std::cout << "Current position: " << f->position() << std::endl;
    }
    catch(...) {
        std::cout << "Exception thrown" << std::endl;
    }

    std::cout << "==== Size ====" << std::endl;
    try {
        auto f = gorc::make_native_read_only_file("tempdir/small.txt");

        std::cout << "Small size: " << f->size() << std::endl;

        auto g = gorc::make_native_read_only_file("tempdir/medium.txt");

        std::cout << "Medium size: " << g->size() << std::endl;
    }
    catch(...) {
        std::cout << "Exception thrown" << std::endl;
    }

    std::cout << "==== Write position ====" << std::endl;

    std::string write_pos_msg = "Hello, World!";
    std::string expected_msg = "Hello, Wqrld!";

    {
        auto f = gorc::make_native_file("tempdir/patched.txt");
        f->write(write_pos_msg.c_str(), write_pos_msg.size());
        f->set_position(8);
        std::cout << "Overwritten character: " << f->position() << std::endl;
        gorc::write<char>(*f, 'q');
        std::cout << "After overwrite: " << f->position() << std::endl;
    }

    {
        auto f = gorc::make_native_read_only_file("tempdir/patched.txt");
        std::string rel;
        rel.resize(13);
        f->read(&rel[0], rel.size());

        if(expected_msg == rel) {
            std::cout << "Messages match" << std::endl;
        }
        else {
            std::cout << "Messages don't match" << std::endl;
        }
    }

    return 0;
}
