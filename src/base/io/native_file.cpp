#include "native_file.h"
#include "exception.h"
#include "base/utility/make_unique.h"
#include <errno.h>
#include <cassert>

using namespace gorc::io;

native_file::native_file(const boost::filesystem::path& filename, bool create_new) {
    file_handle = nullptr;

    if(create_new) {
        if(create(filename)) {
            return;
        }

        throw file_create_exception();
    }
    else {
        if(open(filename)) {
            return;
        }

        throw file_open_exception();
    }

    return;
}

native_file::~native_file() {
    // fclose may fail if buffered data cannot be written to disk.
    // This case can't be meaningfully handled; abort.
    assert(fclose(file_handle) == 0);
}

bool native_file::open(const boost::filesystem::path& filename) {
    file_handle = fopen(filename.native().c_str(), "rb");

    return file_handle != nullptr;
}

bool native_file::create(const boost::filesystem::path& filename) {
    file_handle = fopen(filename.native().c_str(), "wb");

    return file_handle != nullptr;
}

void native_file::read(void* dest, size_t length) {
    size_t len = fread(dest, 1U, length, file_handle);

    if(len != length) {
        throw file_read_exception();
    }

    return;
}

void native_file::write(const void* buffer, size_t length) {
    // fwrite may fail if the buffer is exhausted and fflush fails.
    // This case can't be meaningfully handled; abort.
    assert(fwrite(buffer, 1U, length, file_handle) == length);
}

void native_file::seek(int offset) {
    if(fseek(file_handle, offset, SEEK_CUR) != 0) {
        throw file_seek_exception();
    }
}

void native_file::set_position(size_t offset) {
    fseek(file_handle, static_cast<long>(offset), SEEK_SET);
}

size_t native_file::position() {
    return static_cast<size_t>(ftell(file_handle));
}

size_t native_file::size() {
    auto current_pointer = ftell(file_handle);
    fseek(file_handle, 0, SEEK_END);
    auto fsz = ftell(file_handle);
    fseek(file_handle, current_pointer, SEEK_SET);
    return static_cast<size_t>(fsz);
}

bool native_file::at_end() {
    return feof(file_handle) != 0;
}

std::unique_ptr<file> gorc::io::make_native_file(const boost::filesystem::path& filename) {
    return make_unique<native_file>(filename, true);
}

std::unique_ptr<read_only_file> gorc::io::make_native_read_only_file(const boost::filesystem::path& filename) {
    return make_unique<native_file>(filename, false);
}
