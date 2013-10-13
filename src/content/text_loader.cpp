#include "text_loader.h"
#include "framework/text/source.h"

std::unique_ptr<Gorc::Content::Asset> Gorc::Content::TextLoader::Deserialize(IO::ReadOnlyFile& file, Manager& manager, Diagnostics::Report& report) {
	Text::Source src(file);
	Text::Tokenizer tok(src, report);

	return Parse(tok, manager, report);
}
