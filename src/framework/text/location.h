#pragma once

#include "framework/diagnostics/errorlocation.h"

namespace Gorc {
namespace Text {

class Location {
public:
	const char* filename;
	unsigned int first_line, first_column, last_line, last_column;

	Location();
	Location(const char* filename, unsigned int first_line, unsigned int first_col, unsigned int last_line, unsigned int last_col);
	Location(const Location& loc);

	const Location& operator=(const Location& loc);
	bool operator==(const Location&) const;

	operator Diagnostics::ErrorLocation() const;
};

}
}
