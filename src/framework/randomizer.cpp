#include "randomizer.h"

Gorc::Randomizer::Randomizer() {
	std::random_device rd;
	rng.seed(rd());
}
