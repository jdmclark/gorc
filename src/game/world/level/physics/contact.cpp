#include "contact.h"

gorc::game::world::level::physics::contact::contact(const vector<3>& normal, const vector<3>& velocity)
	: normal(normal), velocity(velocity) {
	return;
}

gorc::game::world::level::physics::contact::contact(const vector<3>& normal, const vector<3>& velocity, int contact_thing_id)
	: normal(normal), velocity(velocity), contact_thing_id(make_maybe(contact_thing_id)) {
	return;
}
