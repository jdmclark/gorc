#pragma once

namespace Gorc {
namespace Flags {

enum class PuppetModeType : int {
	Default = 0,
	AimedWeapon = 1,
	MeleeWeapon = 2,
	SwimmingDefault = 3,
	SwimmingAimedWeapon = 4,
	SwimmingMeleeWeapon = 5
};

}
}
