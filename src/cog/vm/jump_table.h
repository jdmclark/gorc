#pragma once

#include "cog/message_id.h"
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

	inline size_t GetTarget(MessageId id) const {
		return table[static_cast<uint8_t>(id)];
	}
};

}
}
}

