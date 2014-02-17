#pragma once

#include <boost/filesystem/path.hpp>

namespace gorc {
namespace io {

class read_only_file {
public:
    const boost::filesystem::path Filename;

    read_only_file(const boost::filesystem::path& filename);
    virtual ~read_only_file();

    virtual void read(void* dest, size_t size) = 0;

    template <typename T> T read() {
        T retval;
        read(&retval, sizeof(T));
        return retval;
    }

    virtual void seek(long offset) = 0;
    virtual void set_position(size_t offset) = 0;
    virtual size_t get_position() = 0;

    virtual size_t get_size() = 0;
    virtual bool is_end_of_file() = 0;
};

}
}
