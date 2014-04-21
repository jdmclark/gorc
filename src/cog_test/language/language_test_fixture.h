#pragma once

#include "test/test.h"
#include <boost/filesystem/path.hpp>
#include <vector>

#include "base/io/file_system.h"
#include "base/io/native_file_system.h"
#include "framework/diagnostics/stored_report.h"
#include "cog/grammar/instance.h"

class LanguageTestFixture : public test::fixture {
private:
    gorc::io::native_file_system nfs;

public:
    const gorc::io::file_system& FileSystem;
    gorc::diagnostics::stored_report report;

protected:
    LanguageTestFixture(const boost::filesystem::path& BasePath);
    LanguageTestFixture(const gorc::io::file_system& fs);

public:
    virtual void ParseFile(const boost::filesystem::path& file) = 0;

    void PrintErrors() const;
};

#define AssertResult(n_errors, n_warnings) {                                                     \
    expect_eq(report.get_error_count(), static_cast<unsigned int>(n_errors));                    \
    expect_eq(report.get_warning_count(), static_cast<unsigned int>(n_warnings));               \
    if(report.get_error_count() != n_errors || report.get_warning_count() != n_warnings) {        \
        PrintErrors();                                                                            \
    }                                                                                            \
}
