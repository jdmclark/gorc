#pragma once

#include <nullunit/nullunit.h>
#include <boost/filesystem/path.hpp>
#include <vector>

#include "framework/diagnostics/storedreport.h"
#include "cog/grammar/instance.h"

class LanguageTestFixture : public NullUnit::Fixture {
public:
	Gorc::Diagnostics::StoredReport Report;
	const boost::filesystem::path BasePath;

protected:
	LanguageTestFixture(const boost::filesystem::path& BasePath);

public:
	virtual void ParseFile(const boost::filesystem::path& file) = 0;

	void PrintErrors() const;
};

#define AssertResult(n_errors, n_warnings) {												\
	Test_Expect_Eq(Report.GetErrorCount(), n_errors);										\
	Test_Expect_Eq(Report.GetWarningCount(), n_warnings);									\
	if(Report.GetErrorCount() != n_errors || Report.GetWarningCount() != n_warnings) {		\
		PrintErrors();																		\
	}																						\
}
