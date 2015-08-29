#include "memory_file.hpp"
#include <stdexcept>
#include <cmath>

gorc::memory_file::reader::reader(memory_file &mf)
    : mf(mf)
{
    return;
}

size_t gorc::memory_file::reader::read_some(void *dest, size_t size)
{
    char *b = static_cast<char*>(dest);

    size_t amt = std::min(mf.buffer.size() - offset, size);

    for(size_t i = 0; i < amt; ++i, ++offset) {
        b[i] = mf.buffer[offset];
    }

    return amt;
}

void gorc::memory_file::reader::seek(ssize_t off)
{
    ssize_t new_offset = static_cast<ssize_t>(offset) + off;

    if(new_offset < 0 || new_offset > static_cast<ssize_t>(mf.buffer.size())) {
        throw std::range_error("memory_file::seek invalid offset");
    }

    offset = static_cast<size_t>(new_offset);
}

void gorc::memory_file::reader::set_position(size_t off)
{
    if(off > mf.buffer.size()) {
        throw std::range_error("memory_file::set_position invalid offset");
    }

    offset = off;
}

size_t gorc::memory_file::reader::position()
{
    return offset;
}

size_t gorc::memory_file::reader::size()
{
    return mf.buffer.size();
}

bool gorc::memory_file::reader::at_end()
{
    return offset >= mf.buffer.size();
}

gorc::memory_file::writer::writer(memory_file &mf)
    : mf(mf)
{
    return;
}

size_t gorc::memory_file::writer::write_some(void const *src, size_t size)
{
    char const *b = static_cast<char const*>(src);

    for(size_t i = 0; i < size; ++i, ++offset) {
        if(offset < mf.buffer.size()) {
            mf.buffer[offset] = b[i];
        }
        else {
            mf.buffer.push_back(b[i]);
        }
    }

    return size;
}

gorc::memory_file::memory_file()
    : reader_instance(*this)
    , writer_instance(*this)
{
    return;
}

void gorc::memory_file::write(void const *src, size_t size)
{
    writer_instance.write(src, size);
}

size_t gorc::memory_file::write_some(void const *src, size_t size)
{
    return writer_instance.write_some(src, size);
}

void gorc::memory_file::read(void *src, size_t size)
{
    reader_instance.read(src, size);
}

size_t gorc::memory_file::read_some(void *src, size_t size)
{
    return reader_instance.read_some(src, size);
}

void gorc::memory_file::seek(ssize_t offset)
{
    reader_instance.seek(offset);
}

void gorc::memory_file::set_position(size_t offset)
{
    reader_instance.set_position(offset);
}

size_t gorc::memory_file::position()
{
    return reader_instance.position();
}

size_t gorc::memory_file::size()
{
    return reader_instance.size();
}

bool gorc::memory_file::at_end()
{
    return reader_instance.at_end();
}

void gorc::memory_file::copy_to(output_stream &f) const
{
    f.write(buffer.data(), buffer.size());
}
