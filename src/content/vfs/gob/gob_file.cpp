#include "gob_file.h"
#include "framework/io/exception.h"

gorc::content::vfs::gob::gob_file::gob_file(const boost::filesystem::path& gobPath,
        const boost::filesystem::path& filename, size_t chunkOffset, size_t chunkLength)
    : io::read_only_file(filename), file(gobPath, false), chunkOffset(chunkOffset),
      chunkLength(chunkLength), chunkEnd(chunkOffset + chunkLength) {
    file.set_position(chunkOffset);
    return;
}

void gorc::content::vfs::gob::gob_file::read(void* dest, size_t size) {
    if(get_position() + size > chunkLength) {
        throw io::file_read_error_exception();
    }

    file.read(dest, size);
}

void gorc::content::vfs::gob::gob_file::seek(long offset) {
    size_t currentPosition = get_position();
    if((offset < 0 && static_cast<size_t>(std::abs(offset)) > currentPosition)
            || currentPosition + offset >= chunkEnd) {
        throw io::file_invalid_seek_exception();
    }

    file.seek(offset);
}

void gorc::content::vfs::gob::gob_file::set_position(size_t offset) {
    if(offset > chunkLength) {
        throw io::file_invalid_seek_exception();
    }

    file.set_position(chunkOffset + offset);
}

size_t gorc::content::vfs::gob::gob_file::get_position() {
    return file.get_position() - chunkOffset;
}

size_t gorc::content::vfs::gob::gob_file::get_size() {
    return chunkLength;
}

bool gorc::content::vfs::gob::gob_file::is_end_of_file() {
    return get_position() >= chunkLength;
}
