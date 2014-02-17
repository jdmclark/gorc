#pragma once

#include "loader.h"
#include "framework/text/tokenizer.h"

namespace gorc {
namespace content {

class text_loader : public loader {
protected:
    virtual std::unique_ptr<asset> parse(text::tokenizer& t, manager& manager, diagnostics::report& report) = 0;

public:
    virtual std::unique_ptr<asset> deserialize(io::read_only_file& file, manager& manager, diagnostics::report& report) override;
};

}
}
