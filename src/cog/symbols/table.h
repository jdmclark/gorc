#pragma once

#include "symbol.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace Gorc {
namespace Cog {
namespace Symbols {

class SymbolTable
{
private:
	std::vector<Symbol> symbols;
	std::unordered_map<std::string, size_t> symbolMap;
	std::vector<std::unique_ptr<std::string>> strings;

public:
	void AddSymbol(SymbolType type, const std::string& name,
		const VM::Value& defaultValue,
		bool local, const std::string& desc, int mask, int linkid, bool nolink);
	void AddSymbol(SymbolType type, const std::string& name,
		bool local, const std::string& desc, int mask, int linkid, bool nolink);
	void AddSymbol(const std::string& name);

	void ReplaceSymbol(SymbolType type, const std::string& name,
		const VM::Value& defaultValue,
		bool local, const std::string& desc, int mask, int linkid, bool nolink);
	void ReplaceSymbol(SymbolType type, const std::string& name,
		bool local, const std::string& desc, int mask, int linkid, bool nolink);
	void ReplaceSymbol(const std::string& name);

	const Symbol& GetSymbol(size_t index) const;
	const Symbol& GetSymbol(const std::string& name) const;

	bool IsSymbolDefined(size_t index) const;
	bool IsSymbolDefined(const std::string& name) const;

	const char* AddString(const std::string&);

	size_t GetSymbolIndex(const std::string& name) const;

	inline size_t size() const {
		return symbols.size();
	}

	inline std::vector<Symbol>::const_iterator begin() const {
		return symbols.begin();
	}

	inline std::vector<Symbol>::const_iterator end() const {
		return symbols.end();
	}
};

}
}
}
