#pragma once

namespace gorc {
namespace flags {

enum class puppet_mode_type : int {
	Default = 0,
	AimedWeapon = 1,
	MeleeWeapon = 2,
	SwimmingDefault = 3,
	SwimmingAimedWeapon = 4,
	SwimmingMeleeWeapon = 5
};

}
}
