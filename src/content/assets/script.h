#pragma once

#include "framework/content/asset.h"
#include "content/loaders/script_loader.h"
#include "cog/script.h"
#include "cog/instance.h"
#include "framework/utility/pool.h"
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
