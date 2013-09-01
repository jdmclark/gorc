#pragma once

#include <random>

namespace Gorc {

class Randomizer {
private:
	std::default_random_engine rng;

public:
	Randomizer();

	inline operator double() {
		return std::generate_canonical<double, 10>(rng);
	}
};

}
