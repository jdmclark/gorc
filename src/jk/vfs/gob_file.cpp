#include "gob_file.hpp"

gorc::gob_file::gob_file(path const &container_filename,
                         size_t chunk_offset,
                         size_t chunk_length)
    : file(container_filename)
    , chunk_end(chunk_offset + chunk_length)
{
    file.set_position(chunk_offset);
}

size_t gorc::gob_file::read_some(void *dest, size_t size)
{
    size_t read_ceil = chunk_end - file.position();
    size_t actual_want = std::min(read_ceil, size);

    return file.read_some(dest, actual_want);
}

bool gorc::gob_file::at_end()
{
    return file.position() >= chunk_end;
}
