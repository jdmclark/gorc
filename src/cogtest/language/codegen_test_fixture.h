#pragma once

#include "language_test_fixture.h"

#include "cog/symbols/table.h"
#include "cog/verbs/table.h"
#include "cog/script.h"
#include "cog/message_id.h"

#include <string>
#include <unordered_map>

class CodegenTestFixture : public LanguageTestFixture {
private:
	void populate_tables();

public:
	std::unordered_map<std::string, gorc::cog::vm::value> ConstantTable;
	std::unordered_map<std::string, gorc::cog::message_id> MessageTable;
	gorc::cog::verbs::verb_table verb_table;
	gorc::cog::script script;

protected:
	CodegenTestFixture(const boost::filesystem::path& BasePath);
	CodegenTestFixture(const gorc::content::filesystem& fs);

public:
	virtual void ParseFile(const boost::filesystem::path& file);
};
