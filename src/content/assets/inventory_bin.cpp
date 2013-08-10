#include "inventory_bin.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "content/manager.h"
#include <unordered_map>
#include <functional>
#include <type_traits>

namespace Gorc {
namespace Content {
namespace Assets {

using InventoryBinParameterParser = std::function<void(InventoryBin&, Text::Tokenizer&, Content::Manager&, const Cog::Compiler&, Diagnostics::Report&)>;

void InventoryBinCogParser(InventoryBin& tpl, Text::Tokenizer& tok, Content::Manager& manager, const Cog::Compiler& compiler, Diagnostics::Report& report) {
	std::string fn = tok.GetSpaceDelimitedString();
	if(boost::iequals(fn, "none")) {
		tpl.Cog = nullptr;
	}
	else {
		try {
			tpl.Cog = &manager.Load<Script>(fn, compiler);
		}
		catch(...) {
			tpl.Cog = nullptr;
		}
	}
}

static const std::unordered_map<std::string, InventoryBinParameterParser> InventoryBinParameterParserMap {
	{ "cog", &InventoryBinCogParser },
};

}
}
}

void Gorc::Content::Assets::InventoryBin::ParseArgs(Text::Tokenizer& tok, Content::Manager& manager, const Cog::Compiler& compiler, Diagnostics::Report& report) {
	bool oldReportEOL = tok.GetReportEOL();
	tok.SetReportEOL(true);

	Text::Token t;
	while(true) {
		tok.GetToken(t);

		if(t.Type == Text::TokenType::EndOfFile || t.Type == Text::TokenType::EndOfLine) {
			break;
		}
		else {
			tok.AssertPunctuator("=");

			std::transform(t.Value.begin(), t.Value.end(), t.Value.begin(), tolower);
			auto it = InventoryBinParameterParserMap.find(t.Value);
			if(it == InventoryBinParameterParserMap.end()) {
				report.AddWarning("InventoryBin::ParseArgs", boost::str(boost::format("unrecognized bin param \'%s\'") % t.Value), t.Location);
				tok.GetSpaceDelimitedString();
			}
			else {
				it->second(*this, tok, manager, compiler, report);
			}
		}
	}

	tok.SetReportEOL(oldReportEOL);
}
