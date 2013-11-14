#include "contact.h"

gorc::game::world::physics::contact::contact(const vector<3>& position, const vector<3>& normal, const vector<3>& velocity)
	: position(position), normal(normal), velocity(velocity) {
	return;
}
