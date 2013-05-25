#pragma once

#include <cstddef>

namespace Gorc {
namespace Cog {
namespace Verbs {

class VerbTable;

class VerbId {
	friend class VerbTable;

private:
	size_t id;

public:
	VerbId(size_t id);
};

}
}
}
