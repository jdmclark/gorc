#pragma once

#include "base/diagnostics/error_location.h"
#include <boost/filesystem/path.hpp>

namespace gorc {
namespace text {

class location {
public:
    boost::filesystem::path const *filename;
    unsigned int first_line, first_column, last_line, last_column;

    location();
    location(boost::filesystem::path const *filename, unsigned int first_line, unsigned int first_col, unsigned int last_line, unsigned int last_col);
    location(const location& loc);

    const location& operator=(const location& loc);
    bool operator==(const location&) const;

    operator diagnostics::error_location() const;
};

}
}
