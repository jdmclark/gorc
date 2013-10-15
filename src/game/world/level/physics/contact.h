#pragma once

#include "object_data.h"
#include "framework/math/vector.h"

namespace gorc {
namespace game {
namespace world {
namespace level {
namespace physics {

class resting_contact {
public:
	vector<3> normal;

	resting_contact() = default;
	resting_contact(const resting_contact&) = default;
	resting_contact(const vector<3>& normal);
};

class contact {
public:
	const object_data* contact_object = nullptr;
	double alpha;
	vector<3> position;
	vector<3> normal;

	contact() = default;
	contact(const contact&) = default;
	contact(const object_data& ContactObject, double alpha, const vector<3>& position, const vector<3>& normal);
};

class contact_range_consumer {
private:
	bool has_valid_contact = false;
	contact contact;
	float alpha_left = 0.0f;
	float alpha_right = 1.0f;

public:
	contact_range_consumer(float alpha_left = 0.0f, float alpha_right = 1.0f);

	inline operator bool() const {
		return has_valid_contact;
	}

	inline operator const class contact&() const {
		return contact;
	}

	inline contact_range_consumer& operator<<=(const class contact& new_contact) {
		if(new_contact.alpha >= alpha_left && new_contact.alpha <= alpha_right &&
				(!has_valid_contact || contact.alpha < contact.alpha)) {
			contact = new_contact;
			has_valid_contact = true;
		}

		return *this;
	}
};

}
}
}
}
}
