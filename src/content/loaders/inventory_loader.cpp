#include "inventory_loader.h"
#include "content/assets/inventory.h"
#include "framework/diagnostics/helper.h"
#include "framework/io/exception.h"
#include <boost/format.hpp>

const std::vector<boost::filesystem::path> Gorc::Content::Loaders::InventoryLoader::AssetRootPath = { "misc" };

Gorc::Content::Loaders::InventoryLoader::InventoryLoader(Cog::Compiler& compiler) : compiler(compiler) {
	return;
}

std::unique_ptr<Gorc::Content::Asset> Gorc::Content::Loaders::InventoryLoader::Parse(Text::Tokenizer& t,
		Content::Manager& manager, Diagnostics::Report& report) {
	std::unique_ptr<Content::Assets::Inventory> dat(new Content::Assets::Inventory());

	Text::Token tok;
	while(true) {
		t.GetToken(tok);

		if(tok.Type == Text::TokenType::EndOfFile || tok.Value == "end") {
			break;
		}
		else {
			int new_bin_index = t.GetNumber<int>();

			auto rv = dat->Bins.emplace(new_bin_index, Assets::InventoryBin());
			if(!rv.second) {
				report.AddWarning("InventoryLoader::Parse", boost::str(boost::format("multiple bins with id %d") % new_bin_index),
						t.GetInternalTokenLocation());
			}

			Assets::InventoryBin& bin = rv.first->second;

			bin.Name = tok.Value;
			bin.BinId = new_bin_index;
			bin.MinValue = t.GetNumber<int>();
			bin.MaxValue = t.GetNumber<int>();
			bin.Flags = FlagSet<Flags::InventoryFlag>(t.GetNumber<uint32_t>());
			bin.ParseArgs(t, manager, compiler, report);

			dat->BinMap.emplace(bin.Name, new_bin_index);
		}
	}

	return std::unique_ptr<Asset>(std::move(dat));
}
