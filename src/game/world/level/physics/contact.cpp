#include "contact.h"

gorc::game::world::level::physics::contact::contact(const vector<3>& normal, const vector<3>& velocity)
	: normal(normal), velocity(velocity) {
	return;
}
