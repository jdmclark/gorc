#include "script_loader.hpp"
#include "libold/cog/compiler.hpp"
#include "libold/content/assets/script.hpp"

gorc::fourcc const gorc::content::loaders::script_loader::type = "COG"_4CC;

namespace {
    const std::vector<gorc::path> asset_root_path = { "cog" };
}

std::vector<gorc::path> const& gorc::content::loaders::script_loader::get_prefixes() const
{
    return asset_root_path;
}

std::unique_ptr<gorc::asset> gorc::content::loaders::script_loader::deserialize(
        input_stream& file, content_manager&, service_registry const &services) const {
    std::unique_ptr<content::assets::script> script(new content::assets::script());

    services.get<gorc::cog::compiler>().compile(file, script->cogscript);

    return std::unique_ptr<asset>(std::move(script));
}
