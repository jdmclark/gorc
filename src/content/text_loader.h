#pragma once

#include "loader.h"
#include "framework/text/tokenizer.h"

namespace Gorc {
namespace Content {

class TextLoader : public Loader {
protected:
	virtual std::unique_ptr<Asset> Parse(Text::Tokenizer& t, Manager& manager, Diagnostics::Report& report) = 0;

public:
	virtual std::unique_ptr<Asset> Deserialize(IO::ReadOnlyFile& file, Manager& manager, Diagnostics::Report& report) override;
};

}
}
