#include "script_loader.h"
#include "content/assets/script.h"

const std::vector<boost::filesystem::path> gorc::content::loaders::script_loader::asset_root_path = { "cog" };

gorc::content::loaders::script_loader::script_loader(const cog::compiler& compiler)
    : compiler(compiler) {
    return;
}

std::unique_ptr<gorc::content::asset> gorc::content::loaders::script_loader::deserialize(const boost::filesystem::path& filename,
        io::read_only_file& file, content_manager&, diagnostics::report& report) {
    std::unique_ptr<content::assets::script> script(new content::assets::script());

    compiler.compile(filename, file, script->cogscript, report);

    return std::unique_ptr<asset>(std::move(script));
}
