#pragma once

namespace gorc {
namespace flags {

enum class AnimFlag {
	Looping							= 0x1,
	SkipFirstFrame					= 0x2,
	SkipFirstTwoFrames				= 0x4
};

}
}
