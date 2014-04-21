#include "inventory_loader.h"
#include "content/assets/inventory.h"
#include "framework/diagnostics/helper.h"
#include "base/io/exception.h"
#include <boost/format.hpp>

const std::vector<boost::filesystem::path> gorc::content::loaders::inventory_loader::asset_root_path = { "misc" };

gorc::content::loaders::inventory_loader::inventory_loader(cog::compiler& compiler) : compiler(compiler) {
    return;
}

std::unique_ptr<gorc::content::asset> gorc::content::loaders::inventory_loader::parse(text::tokenizer& t,
        content::content_manager& manager, diagnostics::report& report) {
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
                report.add_warning("InventoryLoader::Parse", boost::str(boost::format("multiple bins with id %d") % new_bin_index),
                        t.get_internal_token_location());
            }

            assets::inventory_bin& bin = rv.first->second;

            bin.name = tok.value;
            bin.bin_id = new_bin_index;
            bin.min_value = t.get_number<int>();
            bin.max_value = t.get_number<int>();
            bin.flags = flag_set<flags::inventory_flag>(t.get_number<uint32_t>());
            bin.parse_args(t, manager, compiler, report);

            dat->bin_map.emplace(bin.name, new_bin_index);
        }
    }

    return std::unique_ptr<asset>(std::move(dat));
}
