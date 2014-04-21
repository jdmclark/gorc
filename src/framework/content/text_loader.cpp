#include "text_loader.h"
#include "framework/text/source.h"

std::unique_ptr<gorc::content::asset> gorc::content::text_loader::deserialize(const boost::filesystem::path& filename,
        io::read_only_file& file, content_manager& manager, diagnostics::report& report) {
    text::source src(filename, file);
    text::tokenizer tok(src, report);

    return parse(tok, manager, report);
}
