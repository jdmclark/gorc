#pragma once

#include <cstdio>
#include <boost/filesystem/path.hpp>
#include "file.h"

namespace gorc {
namespace io {

class native_file : public file {
private:
	FILE* hFile;

	bool open(const boost::filesystem::path& filename);
	bool create(const boost::filesystem::path& filename);
	bool close();

public:
	native_file(const boost::filesystem::path& filename, bool createNew = false);
	~native_file();

	virtual void read(void* dest, size_t length) override;
	virtual void write(const void* buffer, size_t length) override;

	virtual void seek(long offset) override;
	virtual void set_position(size_t offset) override;
	virtual size_t get_position() override;

	virtual size_t get_size() override;
	virtual bool is_end_of_file() override;
};

}
}
