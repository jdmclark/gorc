#pragma once

#include <cstdint>
#include "framework/flagset.h"

namespace Gorc {
namespace Content {
namespace Assets {

enum class ActorFlag : uint32_t {
	PitchingChangesHeadPitch 		= 0x1,
	PlayersFieldlight				= 0x4,
	Invulnerability					= 0x8,
	PlayersHeadCentered				= 0x10,
	ExplodesWhenKilled				= 0x20,
	WillNotDrown					= 0x40,
	Invisibility					= 0x80,
	Droid							= 0x100,
	Boss							= 0x200,
	Deaf							= 0x400,
	Blind							= 0x800,
	BlockingAttacks					= 0x2000,
	Screaming						= 0x10000,
	PartiallyImmobilized			= 0x40000,
	CannotShootUnderwater			= 0x80000,
	CannotBeAutoTargeted			= 0x100000,
	TotallyImmobilized				= 0x200000,
	FallingToDeath					= 0x400000,
	NoDisplayHud					= 0x800000,
	DamagesAtFullRate				= 0x1000000,
	CanSeeInDark					= 0x2000000,
	CausesGreenCornerBrackets		= 0x80000000
};

enum class AiModeFlag : uint32_t {
	MovingTowardDestination			= 0x1,
	AttackingTarget					= 0x2,
	SearchingForTarget				= 0x4,
	TurningToFaceTarget				= 0x8,
	HasMoveTarget					= 0x10,
	HasFireTarget					= 0x20,
	Ready							= 0x200,
	HasLineOfSight					= 0x400,
	Fleeing							= 0x800,
	Passive							= 0x1000,
	Disabled						= 0x2000
};

enum class AttachFlag : uint32_t {
	AttachedToWorldSurface			= 0x1,
	AttachedToThingFace				= 0x2,
	AttachedToThing					= 0x4,
	NoMoveRelativeToAttachedThing	= 0x8
};

enum class CollideType : uint32_t {
	NotColliding					= 0x0,
	SphereColliding					= 0x1,
	FaceColliding					= 0x3
};

enum class DamageFlag : uint32_t {
	Impact							= 0x1,
	Energy							= 0x2,
	Explosion						= 0x4,
	Force							= 0x8,
	Saber							= 0x10,
	Drowning						= 0x20
};

enum class ExplosionFlag : uint32_t {
	AnimatedSprite					= 0x1,
	ExplosionIncludesBlastPhase		= 0x2,
	DamageInBlastRadius				= 0x4,
	HasChildExplosions				= 0x8,
	VariableLight					= 0x10,
	NoDamageToShooter				= 0x40,
	RandomDebris					= 0x80,
	FlashBlindsThings				= 0x100
};

enum class GeometryMode : uint32_t {
	NotDrawn						= 0,
	Points							= 1,
	Wireframe						= 2,
	Solid							= 3,
	Textured						= 4,
	Full							= 5
};

enum class InventoryFlag : uint32_t {
	Registered						= 0x1,
	PartOfItemSet					= 0x2,
	Weapon							= 0x4,
	ForcePower						= 0x8,
	Armored							= 0x10,
	AvailableByDefault				= 0x20,
	NotCarriedBetweenLevels			= 0x40,
	DropsOnDeath					= 0x80,
	Hotkeyable						= 0x100
};

enum class GoalFlag : uint32_t {
	DisplayedInObjectives			= 0x1,
	Accomplished					= 0x2
};

enum class ItemFlag : uint32_t {
	RespawnInMultiplayer			= 0x1,
	RespawnInSingleplayer			= 0x2,
	Backpack						= 0x4,
	CtfFlag							= 0x8
};

enum class JkFlag : uint32_t {
	HasSaber						= 0x1,
	SaberCollides					= 0x2,
	SaberIgniting					= 0x4,
	SaberShrinking					= 0x8,
	HasTwoSabers					= 0x10,
	PersuasionTwinkling				= 0x20,
	AttackingInSameKeyframe			= 0x40
};

enum class LightMode : uint32_t {
	FullyLit						= 0,
	NotLit							= 1,
	Diffuse							= 2,
	Gouraud							= 3,
	Gouraud2						= 4,
	Gouraud3						= 5
};

enum class MultiplayerModeFlag : uint32_t {
	TeamPlay						= 0x1,
	NoFriendlyFire					= 0x2,
	CtfComputesOwnScore				= 0x4,
	TimeLimit						= 0x8,
	ScoreLimit						= 0x10,
	NoCustomSkins					= 0x20,
	AutoAssignTeams					= 0x100
};

enum class MultiplayerRespawnCtfFlag : uint32_t {
	StartPlaces16to31				= 0x0000FFFF,
	GoldStarts8to15					= 0xFFFF00FF,
	RedStarts0to7					= 0xFFFFFF00
};

enum class ParticleFlag : uint32_t {
	OutwardExpanding				= 0x1,
	AnimateCel						= 0x2,
	RandomStartCel					= 0x4,
	FadeOutOverTime					= 0x8,
	EmitLight						= 0x10,
	Flipped							= 0x20
};

enum class PhysicsFlag : uint32_t {
	HasGravity						= 0x1,
	UsesThrustToMove				= 0x2,
	AlignOrientationWithSurface		= 0x10,
	BounceOffSurface				= 0x20,
	SticksToFloor					= 0x40,
	SticksToWall					= 0x80,
	UsesRotationalVelocity			= 0x200,
	BanksWhenTurning				= 0x400,
	UsesAngularThrustToRotate		= 0x1000,
	CanFly							= 0x2000,
	AffectedByBlastForce			= 0x4000,
	IsCrouching						= 0x10000,
	DoNotRotateVelocity				= 0x20000,
	PartialGravity					= 0x40000,
	TreadingWater					= 0x100000,
	NotAffectedByThrust				= 0x400000
};

enum class PlayerSuperFlag : uint32_t {
	WeaponSupercharge				= 0x1,
	ShieldSupercharge				= 0x2,
	ForceBoost						= 0x4
};

enum class SectorFlag : uint32_t {
	NoGravity						= 0x1,
	Underwater						= 0x2,
	CogLinked						= 0x4,
	HasThrust						= 0x8,
	HideOnAutomap					= 0x10,
	NoActorEnter					= 0x20,
	Pit								= 0x40,
	HasCollideBox					= 0x1000,
	ShownOnAutoMap					= 0x4000
};

enum class SoundFlag : uint32_t {
	LoopsUntilStopped				= 0x1,
	Ambient							= 0x4,
	PositionInWorldCoordinates		= 0x40,
	PositionMovesWithThing			= 0x80,
	RaisePriority					= 0x100,
	Voice							= 0x10000
};

enum class SurfaceFlag : uint32_t {
	Floor							= 0x1,
	CogLinked						= 0x2,
	Impassable 						= 0x4,
	AiCannotWalkOnFloor				= 0x8,
	DoubleTextureScale				= 0x10,
	HalfTextureScale				= 0x20,
	EighthTextureScale				= 0x40,
	NoFallingDamage					= 0x80,
	HorizonSky						= 0x200,
	CeilingSky						= 0x400,
	Scrolling						= 0x800,
	Icy								= 0x1000,
	VeryIcy							= 0x2000,
	MagSealed						= 0x4000,
	Metal							= 0x10000,
	DeepWater						= 0x20000,
	ShallowWater					= 0x40000,
	Dirt							= 0x80000,
	VeryDeepWater					= 0x100000
};

enum class SurfaceAdjoinFlag : uint32_t {
	Visible 						= 0x1,
	AllowMovement 					= 0x2,
	AllowSound 						= 0x4,
	AllowPlayerOnly 				= 0x8,
	AllowAiOnly 					= 0x10
};

enum class SurfaceAnimationFlag : uint32_t {
	Looping							= 0x1,
	SkipFirstFrame					= 0x2,
	SkipFirstTwoFrames				= 0x4
};

enum class FaceTypeFlag : uint32_t {
	Normal							= 0,
	TwoSided						= 1,
	Translucent						= 2
};

enum class TextureMode : uint32_t {
	AffineMapping					= 0,
	PerspectiveMapping				= 1
};

enum class ThingFlag : uint32_t {
	EmitsLight						= 0x1,
	Dead							= 0x2,
	MagSealed						= 0x4,
	PartOfWorldGeometry				= 0x8,
	Invisible						= 0x10,
	CanStandOn						= 0x40,
	CogLinked						= 0x400,
	NoCrush							= 0x800,
	NotInEasy						= 0x1000,
	NotInMedium						= 0x2000,
	NotInHard						= 0x4000,
	NotInMultiplayer				= 0x8000,
	NotInSingleplayer				= 0x10000,
	ObjectSendingPulse				= 0x20000,
	ObjectSendingTimer				= 0x40000,
	Metal							= 0x400000,
	Dirt							= 0x800000,
	NoSounds						= 0x1000000,
	Underwater						= 0x2000000,
	DestroyedIfEntersWater			= 0x8000000,
	DestroyedIfEntersAir			= 0x10000000,
	GeneratesSplash					= 0x20000000
};

enum class ThingType : uint32_t {
	Camera							= 1,
	Actor							= 2
};

}
}
}
