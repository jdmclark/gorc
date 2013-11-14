#pragma once

#include "filesystem.h"

namespace gorc {
namespace content {

class native_filesystem : public filesystem {
private:
	const boost::filesystem::path basepath;
public:
	native_filesystem(const boost::filesystem::path& basepath);

	virtual std::unique_ptr<gorc::io::read_only_file> open(const boost::filesystem::path& name) const override;
};

}
}
