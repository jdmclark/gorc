#include "native_file.h"
#include "exception.h"

#include <errno.h>

using namespace gorc::io;

native_file::native_file(const boost::filesystem::path& filename, bool createnew)
    : file(filename.native()) {
    hFile = nullptr;

    if(createnew) {
        if(create(filename)) {
            return;
        }

        throw file_create_error_exception();
    }
    else {
        if(open(filename)) {
            return;
        }

        throw file_open_error_exception();
    }

    return;
}

native_file::~native_file() {
    close();
    return;
}

bool native_file::open(const boost::filesystem::path& filename) {
    if(hFile) {
        if(!close()) {
            return false;
        }
    }

    hFile = fopen(filename.native().c_str(), "rb");

    return hFile != nullptr;
}

bool native_file::create(const boost::filesystem::path& filename) {
    if(hFile) {
        if(!close()) {
            return false;
        }
    }

    hFile = fopen(filename.native().c_str(), "wb");

    return hFile != nullptr;
}

bool native_file::close() {
    if(hFile) {
        if(fclose(hFile) != 0) {
            return false;
        }
    }

    hFile = nullptr;
    return true;
}

void native_file::read(void* dest, size_t length) {
    if(hFile) {
        size_t len = fread(dest, 1U, length, hFile);

        if(len != length) {
            throw file_read_error_exception();
        }
    }

    return;
}

void native_file::write(const void* buffer, size_t length) {
    if(hFile) {
        size_t len = fwrite(buffer, 1U, length, hFile);

        if(len != length) {
            throw file_write_error_exception();
        }
    }

    return;
}

void native_file::seek(long offset) {
    if(hFile) {
        if(fseek(hFile, offset, SEEK_CUR) != 0) {
            throw file_invalid_seek_exception();
        }
    }
}

void native_file::set_position(size_t offset) {
    if(hFile) {
        if(fseek(hFile, static_cast<long>(offset), SEEK_SET) != 0) {
            throw file_invalid_seek_exception();
        }
    }
}

size_t native_file::get_position() {
    if(hFile) {
        return static_cast<size_t>(ftell(hFile));
    }

    return 0;
}

size_t native_file::get_size() {
    if(hFile) {
        auto currentPointer = ftell(hFile);
        fseek(hFile, 0, SEEK_END);
        auto fSize = ftell(hFile);
        fseek(hFile, currentPointer, SEEK_SET);
        return static_cast<size_t>(fSize);
    }

    return 0;
}

bool native_file::is_end_of_file() {
    if(hFile) {
        return feof(hFile) != 0;
    }

    return true;
}
