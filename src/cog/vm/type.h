#pragma once

#include <cstdint>

namespace Gorc {
namespace Cog {
namespace VM {

enum class Type : uint8_t {
	Void,
	Integer,
	Float,
	Boolean,
	String,
	Vector
};

}
}
}
