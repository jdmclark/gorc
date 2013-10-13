#pragma once

#include <initializer_list>
#include <type_traits>

namespace Gorc {

template <typename T> class FlagSet {
	using UT = typename std::underlying_type<T>::type;
private:
	UT value;

public:
	FlagSet() : value(0) {
		return;
	}

	explicit FlagSet(UT forced_value) : value(forced_value) {
		return;
	}

	FlagSet(const std::initializer_list<T>& initl) {
		value = 0;
		for(T val : initl) {
			value |= static_cast<UT>(val);
		}
	}

	explicit operator UT() const {
		return value;
	}

	inline bool operator&(T val) const {
		return value & static_cast<UT>(val);
	}

	inline bool operator&(FlagSet val) const {
		return value & val.value;
	}

	inline FlagSet operator+(T val) const {
		return FlagSet(value | static_cast<UT>(val));
	}

	inline const FlagSet& operator+=(T val) {
		value |= static_cast<UT>(val);
		return *this;
	}

	inline FlagSet operator+(FlagSet val) const {
		return FlagSet(value | val.value);
	}

	inline const FlagSet& operator+=(FlagSet val) {
		value |= val.value;
		return *this;
	}

	inline FlagSet operator-(T val) const {
		return FlagSet(value & ~static_cast<UT>(val));
	}

	inline const FlagSet& operator-=(T val) {
		value &= ~static_cast<UT>(val);
		return *this;
	}

	inline FlagSet operator-(FlagSet val) const {
		return FlagSet(value & ~val.value);
	}

	inline const FlagSet& operator-=(FlagSet val) {
		value &= ~val.value;
		return *this;
	}
};

}
