#pragma once

#include "content/asset.hpp"
#include "libold/content/loaders/level_loader.hpp"

#include "math/vector.hpp"
#include "level_header.hpp"
#include "level_adjoin.hpp"
#include "level_surface.hpp"
#include "level_sector.hpp"
#include "script.hpp"
#include "material.hpp"
#include "template.hpp"

#include <unordered_map>

namespace gorc {
namespace content {
namespace assets {

class level : public asset {
public:
    using loader = loaders::level_loader;

    level_header header;
    std::vector<std::tuple<material const*, float, float, std::string>> materials;

    colormap const* master_colormap = nullptr;
    std::vector<colormap const*> colormaps;

    std::vector<script const*> scripts;

    std::vector<std::unique_ptr<std::string>> cog_strings;
    std::vector<std::tuple<script const*, std::vector<cog::vm::value>>> cogs;

    std::vector<vector<3>> vertices;
    std::vector<vector<2>> texture_vertices;
    std::vector<level_adjoin> adjoins;
    std::vector<level_surface> surfaces;
    std::vector<level_sector> sectors;

    std::vector<thing_template> templates;
    std::unordered_map<std::string, int> template_map;
    std::vector<thing_template> things;
};

}
}
}
