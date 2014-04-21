#pragma once

#include "file_system.h"

namespace gorc {
namespace io {

class native_file_system : public file_system {
private:
    const boost::filesystem::path base_path;
public:
    native_file_system(const boost::filesystem::path& base_path);

    virtual std::unique_ptr<read_only_file> open(const boost::filesystem::path& filename) const override;
};

}
}
