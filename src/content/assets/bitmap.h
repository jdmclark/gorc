#pragma once

#include "bitmap_cel.h"
#include "content/loaders/bitmap_loader.h"
#include <vector>
#include <array>
#include <cstdint>

namespace gorc {
namespace content {
namespace assets {

class bitmap : public asset {
public:
	using loader = loaders::bitmap_loader;

	box<2, int> size;
	std::vector<bitmap_cel> cels;
	std::array<vector<3, uint8_t>, 256> color;

	inline void set_color(uint8_t index, const vector<3, uint8_t>& new_color) {
		color[index] = new_color;
	}

	inline const vector<3, uint8_t>& get_color(uint8_t index) const {
		return color[index];
	}
};

}
}
}
