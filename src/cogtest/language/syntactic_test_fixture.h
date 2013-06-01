#pragma once

#include "language_test_fixture.h"

class SyntacticTestFixture : public LanguageTestFixture {
protected:
	using LanguageTestFixture::LanguageTestFixture;

public:
	virtual void ParseFile(const boost::filesystem::path& file);
};
