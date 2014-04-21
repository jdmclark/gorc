#include "sfont.h"

const gorc::content::assets::glyph& gorc::content::assets::sfont::get_glyph(char c) const {
    auto it = glyph_map.find(c);
    if(it == glyph_map.end()) {
        return glyph_map.begin()->second;
    }

    return it->second;
}

int gorc::content::assets::sfont::get_kerning(char, char) const {
    return 0;
}

const gorc::content::assets::texture& gorc::content::assets::sfont::get_texture() const {
    return embedded_bm->cels.front().color;
}

int gorc::content::assets::sfont::line_height() const {
    return height;
}
