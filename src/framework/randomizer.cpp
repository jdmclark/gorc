#include "randomizer.h"

gorc::randomizer::randomizer() {
	std::random_device rd;
	rng.seed(rd());
}
