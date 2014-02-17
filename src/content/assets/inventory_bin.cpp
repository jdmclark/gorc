#include "inventory_bin.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "framework/content/manager.h"
#include <unordered_map>
#include <functional>
#include <type_traits>

namespace gorc {
namespace content {
namespace assets {

using InventoryBinParameterParser = std::function<void(inventory_bin&, text::tokenizer&, content::manager&, const cog::compiler&, diagnostics::report&)>;

void InventoryBinCogParser(inventory_bin& tpl, text::tokenizer& tok, content::manager& manager, const cog::compiler& compiler, diagnostics::report& report) {
    std::string fn = tok.get_space_delimited_string();
    if(boost::iequals(fn, "none")) {
        tpl.cog = nullptr;
    }
    else {
        try {
            tpl.cog = &manager.load<script>(fn, compiler);
        }
        catch(...) {
            report.add_warning("InventoryBin::CogParser",
                    boost::str(boost::format("could not load %s for bin %d") % fn % tpl.bin_id),
                    tok.get_internal_token_location());
            tpl.cog = nullptr;
        }
    }
}

static const std::unordered_map<std::string, InventoryBinParameterParser> InventoryBinParameterParserMap {
    { "cog", &InventoryBinCogParser },
};

}
}
}

void gorc::content::assets::inventory_bin::parse_args(text::tokenizer& tok, content::manager& manager, const cog::compiler& compiler, diagnostics::report& report) {
    bool oldReportEOL = tok.get_report_eol();
    tok.set_report_eol(true);

    text::token t;
    while(true) {
        tok.get_token(t);

        if(t.type == text::token_type::end_of_file || t.type == text::token_type::end_of_line) {
            break;
        }
        else {
            tok.assert_punctuator("=");

            std::transform(t.value.begin(), t.value.end(), t.value.begin(), tolower);
            auto it = InventoryBinParameterParserMap.find(t.value);
            if(it == InventoryBinParameterParserMap.end()) {
                report.add_warning("InventoryBin::ParseArgs", boost::str(boost::format("unrecognized bin param \'%s\'") % t.value), t.location);
                tok.get_space_delimited_string();
            }
            else {
                it->second(*this, tok, manager, compiler, report);
            }
        }
    }

    tok.set_report_eol(oldReportEOL);
}
