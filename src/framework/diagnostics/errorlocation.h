#pragma once

namespace Gorc {
namespace Diagnostics {

class ErrorLocation {
public:
	const char* filename;
	unsigned int first_line, first_column, last_line, last_column;

	ErrorLocation();
	ErrorLocation(const char* filename, unsigned int first_line, unsigned int first_col, unsigned int last_line, unsigned int last_col);
	ErrorLocation(const ErrorLocation& loc);

	const ErrorLocation& operator=(const ErrorLocation& loc);
	bool operator==(const ErrorLocation&) const;
};

}
}
