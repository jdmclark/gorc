#pragma once

namespace gorc {
namespace flags {

enum class sound_subclass_type {
	Reserved,

	create,
	Activate,

	StartMove,
	StopMove,
	Moving,

	LWalkHard,
	RWalkHard,

	LRunHard,
	RRunHard,

	LWalkMetal,
	RWalkMetal,

	LRunMetal,
	RRunMetal,

	LWalkWater,
	RWalkWater,

	LRunWater,
	RRunWater,

	LWalkPuddle,
	RWalkPuddle,

	LRunPuddle,
	RRunPuddle,

	LWalkEarth,
	RWalkEarth,

	LRunEarth,
	RRunEarth,

	EnterWater,
	EnterWaterSlow,
	ExitWater,
	ExitWaterSlow,

	LSwimSurface,
	RSwimSurface,

	TreadSurface,

	LSwimUnder,
	RSwimUnder,

	TreadUnder,

	Jump,
	JumpMetal,
	JumpEarth,
	JumpWater,

	LandHard,
	LandMetal,
	LandWater,
	LandPuddle,
	LandEarth,
	LandHurt,

	HitHard,
	HitMetal,
	HitEarth,

	Deflected,

	ScrapeHard,
	ScrapeMetal,
	ScrapeEarth,

	HitDamaged,

	Falling,

	CorpseHit,

	HurtImpact,
	HurtEnergy,
	HurtFire,
	HurtMagic,
	HurtSpecial,
	Drowning,
	Choking,
	Death1,
	Death2,
	DeathUnder,
	Drowned,
	Splattered,

	Pant,
	Breath,
	Gasp,

	Fire1,
	Fire2,
	Fire3,
	Fire4,

	Curious,
	Alert,
	Idle,
	Gloat,
	Fear,
	Boast,
	Happy,
	Victory,
	Help,
	Flee,
	Search,
	Calm,
	Surprise,

	Reserved1,
	Reserved2,
	Reserved3,
	Reserved4,
	Reserved5,
	Reserved6,
	Reserved7,
	Reserved8
};

}
}
