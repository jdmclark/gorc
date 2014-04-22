#include "font.h"

gorc::content::assets::glyph::glyph(const box<2, int>& size, const box<2, float>& tex_coords)
    : size(size), tex_coords(tex_coords) {
    return;
}

gorc::content::assets::font::~font() {
    return;
}
