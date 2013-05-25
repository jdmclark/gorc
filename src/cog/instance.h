#pragma once

#include "cog/vm/value.h"
#include <vector>

namespace Gorc {
namespace Cog {

class Script;

class Instance {
public:
	const Cog::Script& Script;
	std::vector<VM::Value> Heap;

	Instance(const Cog::Script& Script);
};

}
}
