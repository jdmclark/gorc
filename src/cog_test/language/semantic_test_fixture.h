#pragma once

#include "language_test_fixture.h"

#include "cog/symbols/table.h"
#include "cog/verbs/table.h"

#include <string>
#include <unordered_map>

class SemanticTestFixture : public LanguageTestFixture {
public:
    gorc::cog::symbols::symbol_table symbol_table;
    std::unordered_map<std::string, gorc::cog::vm::value> ConstantTable;
    gorc::cog::verbs::verb_table verb_table;

protected:
    using LanguageTestFixture::LanguageTestFixture;

public:
    virtual void ParseFile(const boost::filesystem::path& file);
};
