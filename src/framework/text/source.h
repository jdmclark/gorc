#pragma once

#include <string>
#include <vector>
#include "framework/io/read_only_file.h"

namespace Gorc {
namespace Text {

class Source {
private:
	std::vector<char> buffer;
	size_t index;

public:
	const std::string Filename;

	Source(IO::ReadOnlyFile& f);
	Source(const std::string& s);

	inline char GetNext() {
		return (index >= buffer.size()) ? '\0' : buffer[index++];
	}
};

}
}
