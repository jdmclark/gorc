#include "log/log.hpp"
#include "inventory_bin.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "libold/base/content/content_manager.hpp"
#include <unordered_map>
#include <functional>
#include <type_traits>

namespace gorc {
namespace content {
namespace assets {

using InventoryBinParameterParser = std::function<void(inventory_bin&, text::tokenizer&, content::content_manager&)>;

void InventoryBinCogParser(inventory_bin& tpl, text::tokenizer& tok, content::content_manager& manager) {
    std::string fn = tok.get_space_delimited_string();
    if(boost::iequals(fn, "none")) {
        tpl.cog = nullptr;
    }
    else {
        try {
            tpl.cog = &manager.load<script>(fn);
        }
        catch(...) {
            diagnostic_context dc(nullptr,
                                  tok.get_internal_token_location().first_line,
                                  tok.get_internal_token_location().first_col);
            LOG_WARNING(format("could not load %d for bin %d") % fn % tpl.bin_id);
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

void gorc::content::assets::inventory_bin::parse_args(text::tokenizer& tok, content::content_manager& manager) {
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
                diagnostic_context dc(nullptr, t.location.first_line, t.location.first_col);
                LOG_WARNING(format("unrecognized bin param '%s'") % t.value);
                tok.get_space_delimited_string();
            }
            else {
                it->second(*this, tok, manager);
            }
        }
    }

    tok.set_report_eol(oldReportEOL);
}
