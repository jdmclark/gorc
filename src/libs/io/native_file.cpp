#include "native_file.hpp"
#include <system_error>
#include "utility/runtime_assert.hpp"
#include <errno.h>

#ifdef PLATFORM_MINGW
#include <windows.h>
#define PLATFORM_FOPEN(fn, op) _wfopen(fn, L##op)
#else
#define PLATFORM_FOPEN(fn, op) fopen(fn, op)
#endif

gorc::native_file::native_file(path const &filename)
{
    file_handle = PLATFORM_FOPEN(filename.c_str(), "wb");
    if(!file_handle) {
        throw std::system_error(errno, std::generic_category());
    }
}

gorc::native_file::~native_file()
{
    // fclose may fail if buffered data cannot be written to disk.
    // This case can't be meaningfully handled; abort.
    runtime_assert(std::fclose(file_handle) == 0, "native_file failed to close handle");
}

size_t gorc::native_file::write_some(void const *src, size_t size)
{
    return std::fwrite(src, 1U, size, file_handle);
}

void gorc::native_file::set_position(size_t offset)
{
    auto res = fseek(file_handle, static_cast<long>(offset), SEEK_SET);
    runtime_assert(res == 0, "native_file::set_position invalid offset");
}

size_t gorc::native_file::position()
{
    return static_cast<size_t>(ftell(file_handle));
}

gorc::native_read_only_file::native_read_only_file(path const &filename)
    : filename(filename)
{
    file_handle = PLATFORM_FOPEN(filename.c_str(), "rb");
    if(!file_handle) {
        throw std::system_error(errno, std::generic_category());
    }
}

gorc::native_read_only_file::~native_read_only_file()
{
    // fclose may fail if buffered data cannot be written to disk.
    // This case can't be meaningfully handled; abort.
    runtime_assert(fclose(file_handle) == 0, "native_read_only_file failed to close handle");
}

size_t gorc::native_read_only_file::read_some(void *dest, size_t length)
{
    return std::fread(dest, 1U, length, file_handle);
}

void gorc::native_read_only_file::seek(ssize_t offset)
{
    if(fseek(file_handle, offset, SEEK_CUR) != 0) {
        throw std::range_error("native_file::seek invalid offset");
    }
}

void gorc::native_read_only_file::set_position(size_t offset)
{
    auto res = fseek(file_handle, static_cast<long>(offset), SEEK_SET);
    runtime_assert(res == 0, "native_file::set_position invalid offset");
}

size_t gorc::native_read_only_file::position()
{
    return static_cast<size_t>(ftell(file_handle));
}

size_t gorc::native_read_only_file::size()
{
    auto current_pointer = ftell(file_handle);
    auto res = fseek(file_handle, 0, SEEK_END);
    runtime_assert(res == 0, "native_read_only_file::size failed to seek to end");
    auto fsz = ftell(file_handle);
    res = fseek(file_handle, current_pointer, SEEK_SET);
    runtime_assert(res == 0, "native_read_only_file::size failed to restore offset");
    return static_cast<size_t>(fsz);
}

bool gorc::native_read_only_file::at_end()
{
    return feof(file_handle) != 0;
}

std::unique_ptr<gorc::native_file> gorc::make_native_file(path const &filename)
{
    return std::make_unique<native_file>(filename);
}

std::unique_ptr<gorc::native_read_only_file> gorc::make_native_read_only_file(path const &filename)
{
    return std::make_unique<native_read_only_file>(filename);
}
