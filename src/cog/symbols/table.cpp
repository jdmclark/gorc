#include "table.h"
#include "exception.h"
#include <string.h>

using namespace Gorc::Cog::Symbols;

SymbolTable::~SymbolTable() {
	for(auto& str : strings) {
		free((void*)str);
	}
}

void SymbolTable::AddSymbol(SymbolType type, const std::string& name,
	const VM::Value& defaultValue, bool local, const std::string& desc, int mask, int linkid, bool nolink) {
	if(IsSymbolDefined(name)) {
		throw SymbolRedefinitionException();
	}

	ReplaceSymbol(type, name, defaultValue, local, desc, mask, linkid, nolink);
}

void SymbolTable::AddSymbol(SymbolType type, const std::string& name,
	bool local, const std::string& desc, int mask, int linkid, bool nolink) {
	if(IsSymbolDefined(name)) {
		throw SymbolRedefinitionException();
	}

	ReplaceSymbol(type, name, local, desc, mask, linkid, nolink);
}

void SymbolTable::AddSymbol(const std::string& name) {
	if(IsSymbolDefined(name)) {
		throw SymbolRedefinitionException();
	}

	ReplaceSymbol(name);
}

void SymbolTable::ReplaceSymbol(SymbolType type, const std::string& name,
	const VM::Value& defaultValue,
	bool local, const std::string& desc, int mask, int linkid, bool nolink) {
	size_t index = symbols.size();
	symbols.push_back(Symbol(type, name, defaultValue, local, desc, mask, linkid, nolink));
	symbolMap.insert(std::make_pair(name, index));
}

void SymbolTable::ReplaceSymbol(SymbolType type, const std::string& name,
	bool local, const std::string& desc, int mask, int linkid, bool nolink) {
	// Generate default value
	VM::Value defaultValue;

	switch(type) {
	case SymbolType::Flex:
		defaultValue = VM::Value(-1.0f);
		break;

	case SymbolType::Float:
		defaultValue = VM::Value(-1.0f);
		break;

	case SymbolType::Int:
		defaultValue = VM::Value(-1);
		break;

	case SymbolType::String:
		defaultValue = VM::Value("");
		break;

	case SymbolType::Vector:
		defaultValue = VM::Value(Math::Vector<float>());
		break;

	default:
		defaultValue = VM::Value(0);
	}

	ReplaceSymbol(type, name, defaultValue, local, desc, mask, linkid, nolink);
}

void SymbolTable::ReplaceSymbol(const std::string& name) {
	ReplaceSymbol(SymbolType::Int, name, true, "", 0, 0, true);
}

const Symbol& SymbolTable::GetSymbol(size_t index) const {
	if(index < symbols.size()) {
		return symbols[index];
	}
	else {
		throw UndefinedSymbolException();
	}
}

const Symbol& SymbolTable::GetSymbol(const std::string& name) const {
	auto it = symbolMap.find(name);
	if(it != symbolMap.end()) {
		return GetSymbol(it->second);
	}
	else {
		throw UndefinedSymbolException();
	}
}

bool SymbolTable::IsSymbolDefined(size_t index) const {
	return index < symbols.size();
}

bool SymbolTable::IsSymbolDefined(const std::string& name) const {
	auto it = symbolMap.find(name);
	return it != symbolMap.end();
}

const char* SymbolTable::AddString(const std::string& str) {
	const char* newstr = strdup(str.c_str());
	strings.push_back(newstr);

	return newstr;
}

size_t SymbolTable::GetSymbolIndex(const std::string& name) const {
	auto it = symbolMap.find(name);
	if(it != symbolMap.end()) {
		return it->second;
	}
	else {
		throw UndefinedSymbolException();
	}
}
