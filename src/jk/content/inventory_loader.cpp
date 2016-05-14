#include "inventory_loader.hpp"
#include "inventory.hpp"
#include "text/token_helpers.hpp"
#include "content/content_manager.hpp"
#include "utility/string_view.hpp"

gorc::fourcc const gorc::inventory_loader::type = "IBIN"_4CC;

namespace {
    std::vector<gorc::path> const asset_root_path = { "misc" };
}

std::vector<gorc::path> const& gorc::inventory_loader::get_prefixes() const
{
    return asset_root_path;
}

namespace {
    using namespace gorc;

    void parse_cog_param(inventory_bin &bin,
                         generic_tokenizer &t,
                         content_manager &content)
    {
        t.extract_string_fragment();
        if(iequal(t.get_value(), "none"_sv)) {
            bin.cog = nothing;
            return;
        }

        try {
            bin.cog = content.load<cog::script>(t.get_value());
        }
        catch(...) {
            diagnostic_context dc(t.get_location());
            LOG_WARNING(format("could not load %s for bin %d") %
                        t.get_value() %
                        bin.bin_id);
            bin.cog = nothing;
        }
    }

    void parse_bin_args(inventory_bin &bin,
                        generic_tokenizer &t,
                        content_manager &content)
    {
        while(true) {
            if(t.get_type() == token_type::end_of_file ||
               t.get_type() == token_type::end_of_line) {
                t.return_newlines(false);
                t.advance();
                break;
            }
            else if(iequal(t.get_value(), "cog"_sv)) {
                t.advance();
                assert_punctuator(t, "=");
                parse_cog_param(bin, t, content);
                t.advance();
            }
            else {
                diagnostic_context dc(t.get_location());
                LOG_FATAL(format("unrecognized bin parameter '%s'") % t.get_value());
            }
        }
    }
}

std::unique_ptr<gorc::asset> gorc::inventory_loader::parse(generic_tokenizer &t,
                                                           content_manager &manager,
                                                           service_registry const &) const
{
    auto dat = std::make_unique<inventory>();

    while(true) {
        if(t.get_type() == token_type::end_of_file) {
            diagnostic_context dc(t.get_location());
            LOG_FATAL("unexpected end of file");
        }
        else if(t.get_value() == "end") {
            break;
        }
        else {
            std::string new_bin_name = t.get_value();
            if(dat->bin_map.find(new_bin_name) != dat->bin_map.end()) {
                diagnostic_context dc(t.get_location());
                LOG_FATAL(format("multiple bins with name %s") % new_bin_name);
            }

            t.advance();

            int new_bin_index = get_number<int>(t);

            auto rv = dat->bins.emplace(new_bin_index, inventory_bin());
            if(!rv.second) {
                diagnostic_context dc(t.get_location());
                LOG_FATAL(format("multiple bins with id %d") % new_bin_index);
            }

            dat->bin_map.emplace(new_bin_name, new_bin_index);

            inventory_bin &bin = rv.first->second;

            bin.name = new_bin_name;
            bin.bin_id = new_bin_index;

            bin.min_value = get_number<int>(t);
            bin.max_value = get_number<int>(t);

            t.return_newlines(true);
            bin.flags = flag_set<inventory_flag>(get_number<uint32_t>(t));

            parse_bin_args(bin, t, manager);
        }
    }

    return std::move(dat);
}
