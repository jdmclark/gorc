#pragma once

#include "cog/messageid.h"
#include <vector>
#include <cstddef>

namespace Gorc {
namespace Cog {
namespace VM {

class JumpTable {
private:
	std::vector<size_t> table;

public:
	JumpTable();

	inline void SetTarget(MessageId id, size_t target) {
		table[static_cast<uint8_t>(id)] = target;
	}

	inline size_t GetTarget(MessageId id) {
		return table[static_cast<uint8_t>(id)];
	}
};

}
}
}

