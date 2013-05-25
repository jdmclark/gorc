#pragma once

#include "framework/math/vector.h"
#include "type.h"

namespace Gorc {
namespace Cog {
namespace VM {

class Value {
private:
	Type type;

	union {
		int Integer;
		float Floating;
		bool Boolean;
		const char* String;
		struct {
			float X, Y, Z;
		} Vector;
	} data;

public:
	Value();

	Value(const Value&);
	const Value& operator=(const Value&);

	Value(int);
	int operator=(int);
	operator int() const;

	Value(float);
	float operator=(float);
	operator float() const;

	Value(bool);
	bool operator=(bool);
	operator bool() const;

	Value(const char*);
	const char* operator=(const char*);
	operator const char*() const;

	Value(const Math::Vector<float>&);
	const Math::Vector<float>& operator=(const Math::Vector<float>&);
	operator Math::Vector<float>() const;

	inline bool IsNumeric() const {
		return type == Type::Integer || type == Type::Float;
	}

	inline Type GetType() const {
		return type;
	}

	inline Value operator+(const Value& v2) const {
		if(type == Type::Float || v2.type == Type::Float) {
			return Value(static_cast<float>(*this) + static_cast<float>(v2));
		}
		else {
			return Value(static_cast<int>(*this) + static_cast<int>(v2));
		}
	}

	inline Value operator-(const Value& v2) const {
		if(type == Type::Float || v2.type == Type::Float) {
			return Value(static_cast<float>(*this) - static_cast<float>(v2));
		}
		else {
			return Value(static_cast<int>(*this) - static_cast<int>(v2));
		}
	}

	inline Value operator*(const Value& v2) const {
		if(type == Type::Float || v2.type == Type::Float) {
			return Value(static_cast<float>(*this) * static_cast<float>(v2));
		}
		else {
			return Value(static_cast<int>(*this) * static_cast<int>(v2));
		}
	}

	inline Value operator/(const Value& v2) const {
		if(type == Type::Float || v2.type == Type::Float) {
			return Value(static_cast<float>(*this) / static_cast<float>(v2));
		}
		else {
			return Value(static_cast<int>(*this) / static_cast<int>(v2));
		}
	}

	inline Value operator%(const Value& v2) const {
		return Value(static_cast<int>(*this) % static_cast<int>(v2));
	}

	inline Value operator&(const Value& v2) const {
		return Value(static_cast<int>(*this) & static_cast<int>(v2));
	}

	inline Value operator|(const Value& v2) const {
		return Value(static_cast<int>(*this) | static_cast<int>(v2));
	}

	inline Value operator^(const Value& v2) const {
		return Value(static_cast<int>(*this) ^ static_cast<int>(v2));
	}

	inline Value operator>(const Value& v2) const {
		if(type == Type::Float || v2.type == Type::Float) {
			return Value(static_cast<float>(*this) > static_cast<float>(v2));
		}
		else {
			return Value(static_cast<int>(*this) > static_cast<int>(v2));
		}
	}

	inline Value operator>=(const Value& v2) const {
		if(type == Type::Float || v2.type == Type::Float) {
			return Value(static_cast<float>(*this) >= static_cast<float>(v2));
		}
		else {
			return Value(static_cast<int>(*this) >= static_cast<int>(v2));
		}
	}

	inline Value operator<(const Value& v2) const {
		if(type == Type::Float || v2.type == Type::Float) {
			return Value(static_cast<float>(*this) < static_cast<float>(v2));
		}
		else {
			return Value(static_cast<int>(*this) < static_cast<int>(v2));
		}
	}

	inline Value operator<=(const Value& v2) const {
		if(type == Type::Float || v2.type == Type::Float) {
			return Value(static_cast<float>(*this) <= static_cast<float>(v2));
		}
		else {
			return Value(static_cast<int>(*this) <= static_cast<int>(v2));
		}
	}

	inline Value operator==(const Value& v2) const {
		if(type == Type::Float || v2.type == Type::Float) {
			return Value(static_cast<float>(*this) == static_cast<float>(v2));
		}
		else if(type == Type::Integer || v2.type == Type::Integer) {
			return Value(static_cast<int>(*this) == static_cast<int>(v2));
		}
		else if(type == Type::Boolean || v2.type == Type::Boolean) {
			return Value(static_cast<bool>(*this) == static_cast<bool>(v2));
		}
		else {
			return Value(false);
		}
	}

	inline Value operator!=(const Value& v2) const {
		if(type == Type::Float || v2.type == Type::Float) {
			return Value(static_cast<float>(*this) != static_cast<float>(v2));
		}
		else if(type == Type::Integer || v2.type == Type::Integer) {
			return Value(static_cast<int>(*this) != static_cast<int>(v2));
		}
		else if(type == Type::Boolean || v2.type == Type::Boolean) {
			return Value(static_cast<bool>(*this) != static_cast<bool>(v2));
		}
		else {
			return Value(false);
		}
	}

	inline Value operator&&(const Value& v2) const {
		return Value(static_cast<bool>(*this) && static_cast<bool>(v2));
	}

	inline Value operator||(const Value& v2) const {
		return Value(static_cast<bool>(*this) || static_cast<bool>(v2));
	}

	inline Value operator+() const {
		return *this;
	}

	inline Value operator-() const {
		if(type == Type::Float) {
			return Value(-static_cast<float>(*this));
		}
		else {
			return Value(-static_cast<int>(*this));
		}
	}

	inline Value operator!() const {
		return Value(!static_cast<bool>(*this));
	}
};

}
}
}
