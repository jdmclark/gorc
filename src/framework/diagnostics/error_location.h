#pragma once

#include <boost/filesystem/path.hpp>

namespace gorc {
namespace diagnostics {

class error_location {
public:
	boost::filesystem::path filename;
	unsigned int first_line, first_column, last_line, last_column;

	error_location();
	error_location(const boost::filesystem::path& filename, unsigned int first_line, unsigned int first_col, unsigned int last_line, unsigned int last_col);
	error_location(const error_location& loc);

	const error_location& operator=(const error_location& loc);
	bool operator==(const error_location&) const;
};

}
}
