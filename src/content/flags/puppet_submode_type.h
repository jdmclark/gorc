#pragma once

namespace Gorc {
namespace Flags {

enum class PuppetSubmodeType : int {
	Stand = 0,
	Walk = 1,
	Run = 2,
	WalkBack = 3,
	StrafeLeft = 4,
	StrafeRight = 5,
	Death = 6,
	Death2 = 7,
	Fire = 8,
	Hit = 9,
	Rising = 10,
	Drop = 11,
	Fall = 12,
	Activate = 13,
	CrouchForward = 14,
	Toss = 15,
	Place = 16,
	Magic = 17,
	Fire2 = 18,
	Choke = 19,
	Leap = 20,
	TurnLeft = 21,
	TurnRight = 22,
	CrouchBack = 23,
	Fidget = 24,
	Fidget2 = 25,

	Land = 26, // Unknown value

	Magic2 = 35,
	Magic3 = 36,
	Windup = 38,

	Holster = 39,
	DrawFists = 40,
	DrawGun = 41,
	DrawSaber = 42,
};

}
}
