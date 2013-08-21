#include "cogcontroller.h"
#include "game/world/level/levelpresenter.h"
#include "game/world/level/thing.h"
#include "game/world/level/levelmodel.h"
#include "game/constants.h"
#include "framework/math/util.h"
#include <iostream>

using namespace Gorc::Math;

void Gorc::Game::World::Level::Gameplay::CogController::Update(int thing_id, double dt) {
	ThingController::Update(thing_id, dt);

	Thing& thing = presenter.Model->Things[thing_id];
	if(thing.PathMoving) {
		UpdateThingPathMoving(thing_id, thing, dt);
	}

	// Update thing attachment motion
	if(thing.AttachFlags & Flags::AttachFlag::AttachedToThing) {
		Thing& attached_thing = presenter.Model->Things[thing.AttachedThing];
		thing.Position = attached_thing.Position - thing.PrevAttachedThingPosition + thing.Position;
		thing.PrevAttachedThingPosition = attached_thing.Position;
	}
}

void Gorc::Game::World::Level::Gameplay::CogController::UpdateThingPathMoving(int thing_id, Thing& thing, double dt) {
	auto target_position_tuple = thing.Frames[thing.NextFrame];
	Vector<3> targetPosition = std::get<0>(target_position_tuple);
	Vector<3> targetOrientation = std::get<1>(target_position_tuple);

	Vector<3> currentPosition = thing.Position;
	Vector<3> currentOrientation = thing.Orientation;

	// PathMoveSpeed seems to be some factor of distance per frame, and Jedi has a different framerate.
	// Use a magic multiple to correct it.
	float dist_len = Math::Length(targetPosition - currentPosition);
	float alpha = RateFactor * dt * thing.PathMoveSpeed / dist_len;
	if(alpha >= 1.0f || dist_len == 0.0f) {
		thing.Position = targetPosition;
		thing.Orientation = targetOrientation;

		// Arrived at next frame. Advance to next.
		thing.CurrentFrame = thing.NextFrame;
		if(thing.CurrentFrame == thing.GoalFrame) {
			thing.PathMoving = false;
			thing.PathMoveSpeed = 0.0f;
			presenter.SoundPresenter.StopFoleyLoop(thing_id);
			presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::StopMove);

			// Dispatch cog messages and resume cogs which are waiting for stop.
			presenter.ScriptPresenter.SendMessageToLinked(Cog::MessageId::Arrived, static_cast<int>(thing_id), Flags::MessageType::Thing);
			presenter.ScriptPresenter.ResumeWaitForStop(thing_id);
		}
		else if(thing.CurrentFrame < thing.GoalFrame) {
			thing.NextFrame = thing.CurrentFrame + 1;
		}
		else {
			thing.NextFrame = thing.CurrentFrame - 1;
		}
	}
	else {
		thing.Position = Math::Lerp(thing.Position, targetPosition, alpha);
		thing.Orientation = Math::Lerp(thing.Orientation, targetOrientation, alpha);
	}
}

void Gorc::Game::World::Level::Gameplay::CogController::CreateControllerData(int thing_id) {
	return;
}
