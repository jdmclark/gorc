#include "randomizer.h"

gorc::utility::randomizer::randomizer() {
	std::random_device rd;
	rng.seed(rd());
}
