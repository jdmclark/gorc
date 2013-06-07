#include "script_loader.h"
#include "content/assets/script.h"

const std::vector<boost::filesystem::path> Gorc::Content::Loaders::ScriptLoader::AssetRootPath = { "cog" };

Gorc::Content::Loaders::ScriptLoader::ScriptLoader(const Cog::Compiler& compiler)
	: Compiler(compiler) {
	return;
}

std::unique_ptr<Gorc::Content::Asset> Gorc::Content::Loaders::ScriptLoader::Deserialize(IO::ReadOnlyFile& file, Manager& manager, Diagnostics::Report& report) {
	std::unique_ptr<Content::Assets::Script> script(new Content::Assets::Script());

	Compiler.Compile(file, script->Script, report);

	return std::unique_ptr<Asset>(std::move(script));
}
