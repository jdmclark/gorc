#include "script_loader.hpp"
#include "libold/cog/compiler.hpp"
#include "libold/content/assets/script.hpp"

const std::vector<boost::filesystem::path> gorc::content::loaders::script_loader::asset_root_path = { "cog" };

std::unique_ptr<gorc::asset> gorc::content::loaders::script_loader::deserialize(
        input_stream& file, content_manager&, service_registry const &services) const {
    std::unique_ptr<content::assets::script> script(new content::assets::script());

    services.get<gorc::cog::compiler>().compile(file, script->cogscript);

    return std::unique_ptr<asset>(std::move(script));
}
