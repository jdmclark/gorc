#pragma once

#include <unordered_map>
#include <memory>
#include "framework/content/asset.h"
#include "framework/content/assets/font.h"
#include "content/assets/bitmap.h"
#include "content/loaders/sfont_loader.h"

namespace gorc {
namespace content {
namespace assets {

class sfont : public font {
public:
    using loader = loaders::sfont_loader;

    std::unordered_map<char, glyph> glyph_map;
    std::unique_ptr<asset> uncast_embedded_bm;
    bitmap* embedded_bm = nullptr;
    int height;

    virtual const glyph& get_glyph(char c) const override;
    virtual int get_kerning(char c, char d) const override;
    virtual const texture& get_texture() const override;

    virtual int line_height() const override;
};

}
}
}
