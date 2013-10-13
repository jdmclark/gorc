#pragma once

#include "object_data.h"
#include "framework/math/vector.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Physics {

class RestingContact {
public:
	Math::Vector<3> Normal;

	RestingContact() = default;
	RestingContact(const RestingContact&) = default;
	RestingContact(const Math::Vector<3>& normal);
};

class Contact {
public:
	const ObjectData* ContactObject = nullptr;
	double Alpha;
	Math::Vector<3> Position;
	Math::Vector<3> Normal;

	Contact() = default;
	Contact(const Contact&) = default;
	Contact(const ObjectData& ContactObject, double Alpha, const Math::Vector<3>& position, const Math::Vector<3>& normal);
};

class ContactRangeConsumer {
private:
	bool has_valid_contact = false;
	Contact contact;
	float alpha_left = 0.0f;
	float alpha_right = 1.0f;

public:
	ContactRangeConsumer(float alpha_left = 0.0f, float alpha_right = 1.0f);

	inline operator bool() const {
		return has_valid_contact;
	}

	inline operator const Contact&() const {
		return contact;
	}

	inline ContactRangeConsumer& operator<<=(const Contact& new_contact) {
		if(new_contact.Alpha >= alpha_left && new_contact.Alpha <= alpha_right &&
				(!has_valid_contact || contact.Alpha < contact.Alpha)) {
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
