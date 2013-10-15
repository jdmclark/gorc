#pragma once

#include <array>
#include <cstdint>
#include "framework/math/vector.h"
#include "content/asset.h"
#include "content/loaders/colormap_loader.h"

namespace gorc {
namespace content {
namespace assets {

class colormap : public asset {
private:
	std::array<vector<3, uint8_t>, 256> color;
	std::array<vector<3, uint8_t>, 256> extra;

public:
	using loader = loaders::colormap_loader;

	inline void set_color(uint8_t index, const vector<3, uint8_t>& new_color) {
		color[index] = new_color;
	}

	inline void set_extra(uint8_t index, const vector<3, uint8_t>& new_color) {
		extra[index] = new_color;
	}

	inline const vector<3, uint8_t>& get_color(uint8_t index) const {
		return color[index];
	}

	inline const vector<3, uint8_t>& get_extra(uint8_t index) const {
		return extra[index];
	}
};

}
}
}
