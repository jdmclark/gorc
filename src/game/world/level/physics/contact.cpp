#include "contact.h"

gorc::game::world::level::physics::resting_contact::resting_contact(const vector<3>& normal)
	: normal(normal) {
	return;
}

gorc::game::world::level::physics::contact::contact(const object_data& ContactObject, double Alpha,
		const vector<3>& position, const vector<3>& normal)
	: contact_object(&ContactObject), alpha(Alpha), position(position), normal(normal) {
	return;
}

gorc::game::world::level::physics::contact_range_consumer::contact_range_consumer(float alpha_left, float alpha_right)
	: alpha_left(alpha_left), alpha_right(alpha_right) {
	return;
}
