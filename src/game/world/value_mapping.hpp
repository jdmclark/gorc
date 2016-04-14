#pragma once

#include "jk/cog/vm/default_value_mapping.hpp"
#include "jk/cog/vm/executor.hpp"
#include "libold/content/assets/level.hpp"
#include "content/content_manager.hpp"

namespace gorc {
namespace game {
namespace world {

class world_value_mapping : public cog::default_value_mapping {
private:
    content_manager &content;
    cog::executor &exec;
    asset_ref<content::assets::level> level;

public:
    world_value_mapping(content_manager &content,
                        cog::executor &exec,
                        asset_ref<content::assets::level> level);

    virtual cog::value operator()(cog::value_type type,
                                  std::string const &value) const override;
};

}
}
}
