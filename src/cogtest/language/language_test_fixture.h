#pragma once

#include <nullunit/nullunit.h>
#include <boost/filesystem/path.hpp>
#include <vector>

#include "framework/content/filesystem.h"
#include "framework/content/native_filesystem.h"
#include "framework/diagnostics/stored_report.h"
#include "cog/grammar/instance.h"

class LanguageTestFixture : public NullUnit::Fixture {
private:
	gorc::content::native_filesystem nfs;

public:
	const gorc::content::filesystem& FileSystem;
	gorc::diagnostics::stored_report report;

protected:
	LanguageTestFixture(const boost::filesystem::path& BasePath);
	LanguageTestFixture(const gorc::content::filesystem& fs);

public:
	virtual void ParseFile(const boost::filesystem::path& file) = 0;

	void PrintErrors() const;
};

#define AssertResult(n_errors, n_warnings) {												\
	Test_Expect_Eq(report.get_error_count(), n_errors);										\
	Test_Expect_Eq(report.get_warning_count(), n_warnings);									\
	if(report.get_error_count() != n_errors || report.get_warning_count() != n_warnings) {		\
		PrintErrors();																		\
	}																						\
}
