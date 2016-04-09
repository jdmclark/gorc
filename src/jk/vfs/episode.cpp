#include "episode.hpp"
#include "log/log.hpp"
#include "text/generic_tokenizer.hpp"
#include "utility/string_search.hpp"
#include "text/token_helpers.hpp"
#include "utility/string_view.hpp"

gorc::episode::episode(input_stream &f) {
    generic_tokenizer t(f);

    if(t.get_type() != token_type::string) {
        diagnostic_context dc(t.get_location());
        LOG_FATAL(format("expected episode name, found '%s'") % t.get_value());
    }

    episode_name = t.get_value();
    t.advance();

    assert_identifier(t, "type");
    type = static_cast<episode_type>(get_number<int>(t));

    assert_identifier(t, "seq");
    size_t num_entries = get_number<size_t>(t);

    while(true) {
        if(t.get_type() == token_type::end_of_file) {
            diagnostic_context dc(t.get_location());
            LOG_FATAL("unexpected end of file");
            return;
        }
        else if(t.get_type() == token_type::identifier &&
                iequal(t.get_value(), "end"_sv)) {
            break;
        }
        else if(t.get_type() == token_type::integer) {
            int line_number = get_number<int>(t);
            assert_punctuator(t, ":");
            int disc = get_number<int>(t);
            int level = get_number<int>(t);

            episode_entry_type entry_type;
            if(iequal(t.get_value(), "cut"_sv)) {
                entry_type = episode_entry_type::cutscene;
            }
            else if(iequal(t.get_value(), "level"_sv)) {
                entry_type = episode_entry_type::level;
            }
            else if(iequal(t.get_value(), "decide"_sv)) {
                entry_type = episode_entry_type::decision;
            }
            else {
                diagnostic_context dc(t.get_location());
                LOG_FATAL(format("unrecognized episode entry type '%s'") % t.get_value());
            }

            t.advance();

            t.extract_string_fragment();
            std::string filename = t.get_value();
            t.advance();

            int lightpow = get_number<int>(t);
            int darkpow = get_number<int>(t);
            int gotoa = get_number<int>(t);
            int gotob = get_number<int>(t);

            entry_map.insert(std::make_pair(line_number, entries.size()));
            entries.emplace_back(line_number,
                                 disc,
                                 level,
                                 entry_type,
                                 filename,
                                 lightpow,
                                 darkpow,
                                 gotoa,
                                 gotob);
        }
        else {
            diagnostic_context dc(t.get_location());
            LOG_FATAL(format("unexpected '%s', expected line number or end") % t.get_value());
        }
    }

    if(num_entries != entries.size()) {
        LOG_WARNING(format("expected %d entries, found %d") %
                    num_entries %
                    entries.size());
    }

    return;
}

std::string const& gorc::episode::get_episode_name() const
{
    return episode_name;
}

gorc::episode_type gorc::episode::get_type() const
{
    return type;
}

size_t gorc::episode::size() const
{
    return entries.size();
}
