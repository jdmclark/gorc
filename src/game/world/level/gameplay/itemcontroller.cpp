#include "itemcontroller.h"
#include "game/world/level/levelpresenter.h"
#include "game/world/level/levelmodel.h"

void Gorc::Game::World::Level::Gameplay::ItemController::CreateControllerData(int thing_id) {
	ThingController::CreateControllerData(thing_id);

	auto& thing = presenter.Model->Things[thing_id];
	thing.ObjectData.CollisionGroup = FlagSet<PhysicsCollideClass> { PhysicsCollideClass::Thing };
	thing.ObjectData.CollisionMask = FlagSet<PhysicsCollideClass> {	PhysicsCollideClass::Player };

	// Disable collision for this object (but keep testing for intersection with player).
	thing.RigidBody->setCollisionFlags(thing.RigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	return;
}

void Gorc::Game::World::Level::Gameplay::ItemController::Taken(int thing_id, int player_id) {
	auto& player_thing = presenter.Model->Things[player_id];
	if(player_thing.Type != Flags::ThingType::Player) {
		return;
	}

	presenter.ScriptPresenter.SendMessageToLinked(Cog::MessageId::Taken,
			thing_id, Flags::MessageType::Thing,
			player_id, Flags::MessageType::Thing);

	// TODO: Don't destroy the thing. Flag it for respawn.
	presenter.DestroyThing(thing_id);
}
