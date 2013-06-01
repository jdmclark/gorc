#pragma once

#include "framework/diagnostics/report.h"
#include "framework/io/readonlyfile.h"
#include "asset.h"
#include <memory>

namespace Gorc {
namespace Content {

class Manager;

class Loader {
public:
	virtual ~Loader();

	virtual std::unique_ptr<Asset> Deserialize(IO::ReadOnlyFile& file, Manager& manager, Diagnostics::Report& report) = 0;
};

}
}
