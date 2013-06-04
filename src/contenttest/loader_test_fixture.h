#pragma once

#include <nullunit/nullunit.h>
#include <boost/filesystem/path.hpp>
#include <vector>

#include "content/filesystem.h"
#include "content/nativefilesystem.h"
#include "framework/diagnostics/storedreport.h"
#include "content/manager.h"

class LoaderTestFixture : public NullUnit::Fixture {
private:
	Gorc::Content::NativeFileSystem nfs;

public:
	const Gorc::Content::FileSystem& FileSystem;
	Gorc::Content::Manager ContentManager;
	Gorc::Diagnostics::StoredReport Report;

protected:
	LoaderTestFixture(const boost::filesystem::path& BasePath);
	LoaderTestFixture(const Gorc::Content::FileSystem& fs);

public:
	template <typename T, typename... ArgT> const T* TryLoad(const boost::filesystem::path& filename, ArgT... args) {
		try {
			return &ContentManager.Load<T>(filename, std::forward(args)...);
		}
		catch(...) {
			return nullptr;
		}
	}

	void PrintErrors() const;
};

#define AssertResult(n_errors, n_warnings) {												\
	Test_Expect_Eq(Report.GetErrorCount(), n_errors);										\
	Test_Expect_Eq(Report.GetWarningCount(), n_warnings);									\
	if(Report.GetErrorCount() != n_errors || Report.GetWarningCount() != n_warnings) {		\
		PrintErrors();																		\
	}																						\
}
