#pragma once

#include "file.h"
#include <cstdio>
#include <memory>
#include <boost/filesystem/path.hpp>

namespace gorc {
namespace io {

class native_file : public file {
    friend std::unique_ptr<file> make_native_file(const std::string& filename);
    friend std::unique_ptr<read_only_file> make_native_read_only_file(const std::string& filename);

private:
    FILE* file_handle;

    bool open(const boost::filesystem::path& filename);
    bool create(const boost::filesystem::path& filename);

public:
    native_file(const boost::filesystem::path& filename, bool create_new = false);
    ~native_file();

    native_file(const native_file&) = delete;
    native_file& operator=(const native_file&) = delete;

    virtual void read(void* dest, size_t size) override;
    virtual void write(const void* buffer, size_t size) override;

    virtual void seek(int offset) override;
    virtual void set_position(size_t offset) override;
    virtual size_t position() override;

    virtual size_t size() override;
    virtual bool at_end() override;
};

std::unique_ptr<file> make_native_file(const boost::filesystem::path& filename);
std::unique_ptr<read_only_file> make_native_read_only_file(const boost::filesystem::path& filename);

}
}
