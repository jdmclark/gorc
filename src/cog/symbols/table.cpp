#include "table.h"
#include "exception.h"
#include <string.h>

using namespace gorc::cog::symbols;

void symbol_table::add_symbol(symbol_type type, const std::string& name,
	const vm::value& defaultvalue, bool local, const std::string& desc, int mask, int linkid, bool nolink) {
	if(is_symbol_defined(name)) {
		throw symbol_redefinition_exception();
	}

	replace_symbol(type, name, defaultvalue, local, desc, mask, linkid, nolink);
}

void symbol_table::add_symbol(symbol_type type, const std::string& name,
	bool local, const std::string& desc, int mask, int linkid, bool nolink) {
	if(is_symbol_defined(name)) {
		throw symbol_redefinition_exception();
	}

	replace_symbol(type, name, local, desc, mask, linkid, nolink);
}

void symbol_table::add_symbol(const std::string& name) {
	if(is_symbol_defined(name)) {
		throw symbol_redefinition_exception();
	}

	replace_symbol(name);
}

void symbol_table::replace_symbol(symbol_type type, const std::string& name,
	const vm::value& defaultvalue,
	bool local, const std::string& desc, int mask, int linkid, bool nolink) {
	size_t index = symbols.size();
	symbols.push_back(symbol(type, name, defaultvalue, local, desc, mask, linkid, nolink));
	symbolMap.insert(std::make_pair(name, index));
}

void symbol_table::replace_symbol(symbol_type type, const std::string& name,
	bool local, const std::string& desc, int mask, int linkid, bool nolink) {
	// Generate default value
	vm::value defaultvalue;

	switch(type) {
	case symbol_type::flex:
		defaultvalue = vm::value(-1.0f);
		break;

	case symbol_type::floating:
		defaultvalue = vm::value(-1.0f);
		break;

	case symbol_type::integer:
		defaultvalue = vm::value(-1);
		break;

	case symbol_type::string:
		defaultvalue = vm::value("");
		break;

	case symbol_type::vector:
		defaultvalue = vm::value(vector<3>());
		break;

	default:
		defaultvalue = vm::value(0);
	}

	replace_symbol(type, name, defaultvalue, local, desc, mask, linkid, nolink);
}

void symbol_table::replace_symbol(const std::string& name) {
	replace_symbol(symbol_type::integer, name, true, "", 0, 0, true);
}

const symbol& symbol_table::get_symbol(size_t index) const {
	if(index < symbols.size()) {
		return symbols[index];
	}
	else {
		throw undefined_symbol_exception();
	}
}

const symbol& symbol_table::get_symbol(const std::string& name) const {
	auto it = symbolMap.find(name);
	if(it != symbolMap.end()) {
		return get_symbol(it->second);
	}
	else {
		throw undefined_symbol_exception();
	}
}

bool symbol_table::is_symbol_defined(size_t index) const {
	return index < symbols.size();
}

bool symbol_table::is_symbol_defined(const std::string& name) const {
	auto it = symbolMap.find(name);
	return it != symbolMap.end();
}

const char* symbol_table::add_string(const std::string& str) {
	strings.emplace_back(new std::string(str));
	return strings.back()->data();
}

size_t symbol_table::get_symbol_index(const std::string& name) const {
	auto it = symbolMap.find(name);
	if(it != symbolMap.end()) {
		return it->second;
	}
	else {
		throw undefined_symbol_exception();
	}
}
