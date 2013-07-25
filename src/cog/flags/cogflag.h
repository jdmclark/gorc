#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class CogFlag : unsigned int {
	Local = 0x40,
	Server = 0x80,
	Global = 0x100,
	NoSync = 0x200
};

}
}
