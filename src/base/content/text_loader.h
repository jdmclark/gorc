#pragma once

#include "loader.h"
#include "base/text/tokenizer.h"

namespace gorc {
namespace content {

class text_loader : public loader {
protected:
    virtual std::unique_ptr<asset> parse(text::tokenizer& t, content_manager& manager, diagnostics::report& report) = 0;

public:
    virtual std::unique_ptr<asset> deserialize(const boost::filesystem::path& filename,
            io::read_only_file& file, content_manager& manager, diagnostics::report& report) override;
};

}
}
