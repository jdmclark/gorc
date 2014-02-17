#pragma once

#include "test/test.h"
#include <boost/filesystem/path.hpp>
#include <vector>

#include "framework/content/filesystem.h"
#include "framework/content/native_filesystem.h"
#include "framework/diagnostics/stored_report.h"
#include "framework/content/manager.h"
#include "cog/compiler.h"

class LoaderTestFixture : public test::fixture {
private:
    gorc::content::native_filesystem nfs;

    void CreateMockCogverbs();

public:
    const gorc::content::filesystem& FileSystem;
    gorc::content::manager contentmanager;
    gorc::diagnostics::stored_report report;
    gorc::cog::verbs::verb_table verb_table;
    gorc::cog::compiler compiler;

protected:
    LoaderTestFixture(const boost::filesystem::path& BasePath);
    LoaderTestFixture(const gorc::content::filesystem& fs);

public:
    template <typename T, typename... ArgT> const T* TryLoad(const boost::filesystem::path& filename, ArgT... args) {
        try {
            return &contentmanager.load<T>(filename, args...);
        }
        catch(...) {
            return nullptr;
        }
    }

    void PrintErrors() const;
};

#define AssertResult(n_errors, n_warnings) {                                                \
    expect_eq(report.get_error_count(), n_errors);                                        \
    expect_eq(report.get_warning_count(), n_warnings);                                    \
    if(report.get_error_count() != n_errors || report.get_warning_count() != n_warnings) {        \
        PrintErrors();                                                                        \
    }                                                                                        \
}
