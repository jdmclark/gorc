#pragma once

#include "framework/content/asset.h"
#include "framework/math/box.h"
#include "framework/content/assets/texture.h"
#include <numeric>
#include <tuple>

namespace gorc {
namespace content {
namespace assets {

class glyph {
public:
	box<2, int> size;
	box<2, float> tex_coords;

	glyph(const box<2, int>& glyph_size, const box<2, float>& glyph_tx_coords);
};

class font : public asset {
public:
	virtual ~font();

	virtual const glyph& get_glyph(char c) const = 0;
	virtual int get_kerning(char c, char d) const = 0;
	virtual const texture& get_texture() const = 0;

	virtual int line_height() const = 0;

	template <typename RangeT> box<2, int> measure_text(RangeT begin, RangeT end) const {
		int width = 0;
		int height = 0;

		int line_width = 0;

		char prev_c = 0;
		for(auto it = begin; it != end; ++it) {
			char c = *it;

			if(c == '\n') {
				line_width = 0;
				height += line_height();
			}
			else {
				line_width += get_kerning(prev_c, c);
				prev_c = c;

				const auto& glyph = get_glyph(c);

				line_width += get_size<0>(glyph.size);
				width = std::max(width, line_width);
			}
		}

		height += line_height();

		return make_box(make_vector(0, 0), make_vector(width, height));
	}
};

}
}
}
