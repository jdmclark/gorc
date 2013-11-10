#pragma once

#include "framework/diagnostics/error_location.h"

namespace gorc {
namespace text {

class location {
public:
	const char* filename;
	unsigned int first_line, first_column, last_line, last_column;

	location();
	location(const char* filename, unsigned int first_line, unsigned int first_col, unsigned int last_line, unsigned int last_col);
	location(const location& loc);

	const location& operator=(const location& loc);
	bool operator==(const location&) const;

	operator diagnostics::error_location() const;
};

}
}
