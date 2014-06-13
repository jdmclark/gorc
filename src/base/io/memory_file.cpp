#include "memory_file.h"
#include "exception.h"
#include <cmath>

gorc::io::memory_file::memory_file() {
    return;
}

void gorc::io::memory_file::read(void* dest, size_t size) {
    char* b = static_cast<char*>(dest);

    if(offset + size > buffer.size()) {
        throw file_read_exception();
    }

    for(size_t i = 0; i < size; ++i, ++offset) {
        b[i] = buffer[offset];
    }
}

void gorc::io::memory_file::write(const void* dest, size_t size) {
    const char* b = static_cast<const char*>(dest);

    for(size_t i = 0; i < size; ++i, ++offset) {
        if(offset < buffer.size()) {
            buffer[offset] = b[i];
        }
        else {
            buffer.push_back(b[i]);
        }
    }
}

void gorc::io::memory_file::seek(int off) {
    if((off < 0 && static_cast<size_t>(std::abs(off)) > offset) || offset + off > buffer.size()) {
        throw file_seek_exception();
    }

    offset += off;
}

void gorc::io::memory_file::set_position(size_t off) {
    if(off > buffer.size()) {
        throw file_seek_exception();
    }

    offset = off;
}

size_t gorc::io::memory_file::position() {
    return offset;
}

size_t gorc::io::memory_file::size() {
    return buffer.size();
}

bool gorc::io::memory_file::at_end() {
    return offset >= buffer.size();
}
