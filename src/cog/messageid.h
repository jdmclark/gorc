#pragma once

#include <cstdint>

namespace Gorc {
namespace Cog {

enum class MessageId : uint8_t {
	Global0			= 0,
	Activated		= 1,
	Removed			= 2,
	Startup			= 3,
	Timer			= 4,
	Blocked			= 5,
	Entered			= 6,
	Exited			= 7,
	Crossed			= 8,
	Sighted			= 9,
	Damaged			= 10,
	Arrived			= 11,
	Killed			= 12,
	Pulse			= 13,
	Touched			= 14,
	Created			= 15,
	Loading			= 16,
	Selected		= 17,
	Deselected		= 18,
	Autoselect		= 19,
	Changed			= 20,
	Deactivated		= 21,
	Shutdown		= 22,
	Respawn			= 23,
	AiEvent			= 24,
	Skill			= 25,
	Taken			= 26,
	User0			= 27,
	User1			= 28,
	User2			= 29,
	User3			= 30,
	User4			= 31,
	User5			= 32,
	User6			= 33,
	User7			= 34,
	NewPlayer		= 35,
	Fire			= 36,
	Join			= 37,
	Leave			= 38,
	Splash			= 39,
	Trigger			= 40
};

}
}
