#include "inventory_loader.hpp"
#include "libold/content/assets/inventory.hpp"
#include <boost/format.hpp>

gorc::fourcc const gorc::content::loaders::inventory_loader::type = "IBIN"_4CC;

namespace {
    const std::vector<gorc::path> asset_root_path = { "misc" };
}

std::vector<gorc::path> const& gorc::content::loaders::inventory_loader::get_prefixes() const
{
    return asset_root_path;
}

std::unique_ptr<gorc::asset> gorc::content::loaders::inventory_loader::parse(text::tokenizer& t,
        content_manager& manager, service_registry const &) const {
    std::unique_ptr<content::assets::inventory> dat(new content::assets::inventory());

    text::token tok;
    while(true) {
        t.get_token(tok);

        if(tok.type == text::token_type::end_of_file || tok.value == "end") {
            break;
        }
        else {
            int new_bin_index = t.get_number<int>();

            auto rv = dat->bins.emplace(new_bin_index, assets::inventory_bin());
            if(!rv.second) {
                diagnostic_context dc(nullptr,
                                      t.get_internal_token_location().first_line,
                                      t.get_internal_token_location().first_col);
                LOG_WARNING(format("multiple bins with id %d") % new_bin_index);
            }

            assets::inventory_bin& bin = rv.first->second;

            bin.name = tok.value;
            bin.bin_id = new_bin_index;
            bin.min_value = t.get_number<int>();
            bin.max_value = t.get_number<int>();
            bin.flags = flag_set<flags::inventory_flag>(t.get_number<uint32_t>());
            bin.parse_args(t, manager);

            dat->bin_map.emplace(bin.name, new_bin_index);
        }
    }

    return std::unique_ptr<asset>(std::move(dat));
}
