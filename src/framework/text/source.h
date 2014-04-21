#pragma once

#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>
#include "base/io/read_only_file.h"

namespace gorc {
namespace text {

class source {
private:
    std::vector<char> buffer;
    size_t index;

public:
    const boost::filesystem::path filename;

    source(const boost::filesystem::path& filename, io::read_only_file& f);
    source(const std::string& s);

    inline char get_next() {
        return (index >= buffer.size()) ? '\0' : buffer[index++];
    }
};

}
}
