#include "text_loader.h"
#include "framework/text/source.h"

std::unique_ptr<gorc::content::asset> gorc::content::text_loader::deserialize(io::read_only_file& file, manager& manager, diagnostics::report& report) {
    text::source src(file);
    text::tokenizer tok(src, report);

    return parse(tok, manager, report);
}
