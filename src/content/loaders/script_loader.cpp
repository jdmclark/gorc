#include "script_loader.h"
#include "content/assets/script.h"

Gorc::Content::Loaders::ScriptLoader::ScriptLoader(const Cog::Compiler& compiler)
	: Compiler(compiler) {
	return;
}

std::unique_ptr<Gorc::Content::Asset> Gorc::Content::Loaders::ScriptLoader::Deserialize(IO::ReadOnlyFile& file, Manager& manager, Diagnostics::Report& report) {
	std::unique_ptr<Content::Assets::Script> script(new Content::Assets::Script());

	Compiler.Compile(file, script->Script, report);

	return std::move(script);
}
