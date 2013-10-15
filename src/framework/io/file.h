#pragma once

#include "read_only_file.h"

namespace gorc {
namespace io {

class file : public read_only_file {
public:
	file(const boost::filesystem::path& filename);
	virtual ~file();

	virtual void write(const void* buffer, size_t size) = 0;

	template <typename T> void write(const T& value) {
		write(&value, sizeof(T));
	}
};

}
}
