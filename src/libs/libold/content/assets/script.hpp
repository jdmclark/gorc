#pragma once

#include "content/asset.hpp"
#include "libold/content/loaders/script_loader.hpp"
#include "libold/cog/script.hpp"
#include "libold/cog/instance.hpp"
#include "libold/base/utility/pool.hpp"
#include <unordered_map>
#include <string>

namespace gorc {
namespace content {

class content_manager;

namespace assets {

class colormap;
class level_adjoin;
class level_sector;
class level_surface;

class script : public asset {
public:
    using loader = loaders::script_loader;

    cog::script cogscript;
};

}
}
}
