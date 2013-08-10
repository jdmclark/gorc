#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include "framework/math/vector.h"
#include "content/asset.h"
#include "content/loaders/inventory_loader.h"
#include "inventory_bin.h"

namespace Gorc {
namespace Content {
namespace Assets {

class Inventory : public Asset {
public:
	using Loader = Loaders::InventoryLoader;

	std::unordered_map<std::string, int> BinMap;
	std::unordered_map<int, InventoryBin> Bins;

	inline const InventoryBin& GetBin(const std::string& name) const {
		auto it = BinMap.find(name);
		if(it != BinMap.end()) {
			auto jt = Bins.find(it->second);
			if(jt != Bins.end()) {
				return jt->second;
			}
		}

		throw std::exception();
	}
};

}
}
}
