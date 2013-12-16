#pragma once

#include "framework/content/assets/texture.h"

namespace gorc {
namespace content {
namespace assets {

class bitmap_cel {
public:
	texture color;

	bitmap_cel(GLuint color, const box<2, int>& m_size);
	bitmap_cel(const bitmap_cel&) = delete;
	bitmap_cel(bitmap_cel&&) = default;
};

}
}
}
