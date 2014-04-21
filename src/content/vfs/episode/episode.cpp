#include "episode.h"
#include "exception.h"
#include "framework/text/source.h"
#include "framework/text/tokenizer.h"
#include "base/io/exception.h"
#include "framework/diagnostics/report.h"
#include "framework/diagnostics/helper.h"
#include <boost/algorithm/string.hpp>

gorc::content::vfs::episode::episode::episode(io::read_only_file& file, diagnostics::report& report) {
    text::source src("episode.jk", file);
    text::tokenizer tok(src, report);
    text::token t;

    EpisodeName = tok.get_string_literal();

    tok.assert_identifier("TYPE");
    type = static_cast<episode_type>(tok.get_number<int>());

    tok.assert_identifier("SEQ");
    int nEntries = tok.get_number<int>();

    while(true) {
        tok.get_token(t);
        if(t.type == text::token_type::end_of_file) {
            diagnostics::helper::unexpected_end_of_file(report, "episode", t.location);
            return;
        }
        else if(t.type == text::token_type::identifier && t.value == "end") {
            return;
        }
        else if(t.type == text::token_type::integer) {
            int number = t.get_numeric_value<int>();
            tok.assert_punctuator(":");
            int disc = tok.get_number<int>();
            int level = tok.get_number<int>();

            entry_type entryType;

            std::string entryToken = tok.get_identifier();
            if(boost::iequals(entryToken, "CUT")) {
                entryType = entry_type::cutscene;
            }
            else if(boost::iequals(entryToken, "LEVEL")) {
                entryType = entry_type::level;
            }
            else if(boost::iequals(entryToken, "DECIDE")) {
                entryType = entry_type::decision;
            }
            else {
                report.add_error("episode", "unrecognized episode entry type", tok.get_internal_token_location());
                throw io::file_corrupt_exception();
            }

            std::string filename = tok.get_space_delimited_string();
            int lightpow = tok.get_number<int>();
            int darkpow = tok.get_number<int>();
            int gotoa = tok.get_number<int>();
            int gotob = tok.get_number<int>();

            entryMap.insert(std::make_pair(number, entries.size()));
            entries.emplace_back(number, disc, level, entryType, filename, lightpow, darkpow, gotoa, gotob);
        }
        else {
            diagnostics::helper::expected(report, "episode", "entry number or end", t.location);
            throw io::file_corrupt_exception();
        }
    }

    return;
}

const gorc::content::vfs::episode::entry& gorc::content::vfs::episode::episode::get_entry(int LineNumber) const {
    auto it = entryMap.find(LineNumber);
    if(it == entryMap.end()) {
        throw line_does_not_exist_exception();
    }

    return entries[it->second];
}
