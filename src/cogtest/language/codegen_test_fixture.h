#pragma once

#include "language_test_fixture.h"

#include "cog/symbols/table.h"
#include "cog/verbs/table.h"
#include "cog/script.h"
#include "cog/messageid.h"

#include <string>
#include <unordered_map>

class CodegenTestFixture : public LanguageTestFixture {
public:
	Gorc::Cog::Symbols::SymbolTable SymbolTable;
	std::unordered_map<std::string, Gorc::Cog::VM::Value> ConstantTable;
	std::unordered_map<std::string, Gorc::Cog::MessageId> MessageTable;
	Gorc::Cog::Verbs::VerbTable VerbTable;
	Gorc::Cog::Script Script;

protected:
	CodegenTestFixture(const boost::filesystem::path& BasePath);

public:
	virtual void ParseFile(const boost::filesystem::path& file);
};
