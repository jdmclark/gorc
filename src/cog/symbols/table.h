#pragma once

#include "symbol.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace gorc {
namespace cog {
namespace symbols {

class symbol_table
{
private:
	std::vector<symbol> symbols;
	std::unordered_map<std::string, size_t> symbolMap;
	std::vector<std::unique_ptr<std::string>> strings;

public:
	void add_symbol(symbol_type type, const std::string& name,
		const vm::value& defaultvalue,
		bool local, const std::string& desc, int mask, int linkid, bool nolink);
	void add_symbol(symbol_type type, const std::string& name,
		bool local, const std::string& desc, int mask, int linkid, bool nolink);
	void add_symbol(const std::string& name);

	void replace_symbol(symbol_type type, const std::string& name,
		const vm::value& defaultvalue,
		bool local, const std::string& desc, int mask, int linkid, bool nolink);
	void replace_symbol(symbol_type type, const std::string& name,
		bool local, const std::string& desc, int mask, int linkid, bool nolink);
	void replace_symbol(const std::string& name);

	const symbol& get_symbol(size_t index) const;
	const symbol& get_symbol(const std::string& name) const;

	bool is_symbol_defined(size_t index) const;
	bool is_symbol_defined(const std::string& name) const;

	const char* add_string(const std::string&);

	size_t get_symbol_index(const std::string& name) const;

	inline size_t size() const {
		return symbols.size();
	}

	inline std::vector<symbol>::const_iterator begin() const {
		return symbols.begin();
	}

	inline std::vector<symbol>::const_iterator end() const {
		return symbols.end();
	}
};

}
}
}
