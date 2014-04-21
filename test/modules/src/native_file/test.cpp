#include "base/io/native_file.h"
#include "base/io/exception.h"
#include <vector>
#include <iostream>
#include <cstring>

int main(int, char**) {
    std::cout << "==== Simple create/read ====" << std::endl;
    try {
        std::string str = "Hello, World!";

        {
            auto g = gorc::io::make_native_file("tempdir/simple.txt");
            g->write(str.data(), str.size());
        }

        auto f = gorc::io::make_native_read_only_file("tempdir/simple.txt");

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
        catch(const gorc::io::file_read_exception& e) {
            std::cout << "file read exception thrown" << std::endl;
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
        auto f = gorc::io::make_native_file("tempdir/existing.txt");

        std::string str = "Contents overwritten\n";
        f->write(str.data(), str.size());
    }
    catch(...) {
        std::cout << "Exception thrown" << std::endl;
    }

    std::cout << "==== Truncate failure ====" << std::endl;
    try {
        auto f = gorc::io::make_native_file("tempdir/write_denied.txt");
        std::cout << "Exception not thrown" << std::endl;
    }
    catch(const gorc::io::file_create_exception&) {
        std::cout << "File create exception thrown" << std::endl;
    }
    catch(...) {
        std::cout << "Unknown exception thrown" << std::endl;
    }

    std::cout << "==== Open failure ====" << std::endl;
    try {
        auto f = gorc::io::make_native_read_only_file("tempdir/read_denied.txt");
        std::cout << "Exception not thrown" << std::endl;
    }
    catch(const gorc::io::file_open_exception&) {
        std::cout << "File open exception thrown" << std::endl;
    }
    catch(...) {
        std::cout << "Unknown exception thrown" << std::endl;
    }

    std::cout << "==== Read failure ====" << std::endl;
    try {
        auto f = gorc::io::make_native_read_only_file("tempdir/small.txt");
        std::vector<char> data;
        data.resize(1024);
        f->read(data.data(), 1024);
        std::cout << "No exception thrown" << std::endl;
    }
    catch(const gorc::io::file_read_exception&) {
        std::cout << "File read exception thrown" << std::endl;
    }
    catch(...) {
        std::cout << "Unknown exception thrown" << std::endl;
    }

    std::cout << "==== Seek failure ====" << std::endl;
    try {
        auto f = gorc::io::make_native_read_only_file("tempdir/small.txt");
        f->seek(-1024);
        std::cout << "No exception thrown" << std::endl;
    }
    catch(const gorc::io::file_seek_exception&) {
        std::cout << "File seek exception thrown" << std::endl;
    }
    catch(...) {
        std::cout << "Unknown exception thrown" << std::endl;
    }

    std::cout << "==== Position ====" << std::endl;
    try {
        auto f = gorc::io::make_native_read_only_file("tempdir/small.txt");

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
        auto f = gorc::io::make_native_read_only_file("tempdir/small.txt");

        std::cout << "Small size: " << f->size() << std::endl;

        auto g = gorc::io::make_native_read_only_file("tempdir/medium.txt");

        std::cout << "Medium size: " << g->size() << std::endl;
    }
    catch(...) {
        std::cout << "Exception thrown" << std::endl;
    }

    return 0;
}
