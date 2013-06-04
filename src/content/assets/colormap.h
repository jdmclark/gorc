#pragma once

#include <array>
#include <cstdint>
#include "framework/math/vector.h"
#include "content/asset.h"
#include "content/loaders/colormap_loader.h"

namespace Gorc {
namespace Content {
namespace Assets {

class Colormap : public Asset {
private:
	std::array<Math::Vector<3, uint8_t>, 256> Color;
	std::array<Math::Vector<3, uint8_t>, 256> Extra;

public:
	using Loader = Loaders::ColormapLoader;

	inline void SetColor(uint8_t index, const Math::Vector<3, uint8_t>& color) {
		Color[index] = color;
	}

	inline void SetExtra(uint8_t index, const Math::Vector<3, uint8_t>& color) {
		Extra[index] = color;
	}

	inline const Math::Vector<3, uint8_t>& GetColor(uint8_t index) const {
		return Color[index];
	}

	inline const Math::Vector<3, uint8_t>& GetExtra(uint8_t index) const {
		return Extra[index];
	}
};

}
}
}
