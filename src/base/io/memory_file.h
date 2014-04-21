#pragma once

#include "file.h"
#include <vector>

namespace gorc {
namespace io {

class memory_file : public file {
private:
    std::vector<char> buffer;
    size_t offset = 0;

public:
    memory_file();

    virtual void read(void* dest, size_t size) override;
    virtual void write(const void* dest, size_t size) override;

    virtual void seek(int offset) override;
    virtual void set_position(size_t offset) override;
    virtual size_t position() override;

    virtual size_t size() override;
    virtual bool at_end() override;
};

}
}
