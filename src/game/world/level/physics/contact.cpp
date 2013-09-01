#include "contact.h"

Gorc::Game::World::Level::Physics::RestingContact::RestingContact(const Math::Vector<3>& normal)
	: Normal(normal) {
	return;
}

Gorc::Game::World::Level::Physics::Contact::Contact(const ObjectData& ContactObject, double Alpha,
		const Math::Vector<3>& position, const Math::Vector<3>& normal)
	: ContactObject(&ContactObject), Alpha(Alpha), Position(position), Normal(normal) {
	return;
}

Gorc::Game::World::Level::Physics::ContactRangeConsumer::ContactRangeConsumer(float alpha_left, float alpha_right)
	: alpha_left(alpha_left), alpha_right(alpha_right) {
	return;
}
