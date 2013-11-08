#include "shape.h"

gorc::game::world::level::physics::sphere::sphere(const vector<3>& position, float radius)
	: position(position), radius(radius) {
	return;
}

gorc::game::world::level::physics::swept_sphere::swept_sphere(const segment& position, float radius)
	: position(position), radius(radius) {
	return;
}
