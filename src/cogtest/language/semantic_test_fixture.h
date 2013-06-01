#pragma once

#include "language_test_fixture.h"

#include "cog/symbols/table.h"
#include "cog/verbs/table.h"

#include <string>
#include <unordered_map>

class SemanticTestFixture : public LanguageTestFixture {
public:
	Gorc::Cog::Symbols::SymbolTable SymbolTable;
	std::unordered_map<std::string, Gorc::Cog::VM::Value> ConstantTable;
	Gorc::Cog::Verbs::VerbTable VerbTable;

protected:
	using LanguageTestFixture::LanguageTestFixture;

public:
	virtual void ParseFile(const boost::filesystem::path& file);
};
