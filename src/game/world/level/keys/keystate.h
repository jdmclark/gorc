#pragma once

#include "content/assets/animation.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Keys {

class KeyState {
public:
	unsigned int MixId;
	Content::Assets::Animation const* Animation = nullptr;
	double AnimationTime;
	double CurrentFrame;
	double Speed;
	FlagSet<Flags::KeyFlag> Flags;

	int HighPriority, LowPriority, BodyPriority;
};

}
}
}
}
}
