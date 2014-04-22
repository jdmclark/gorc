#include "test/test.h"
#include "content/vfs/gob/gob_container.h"
#include "base/io/native_file.h"
#include "base/text/source.h"
#include "base/text/tokenizer.h"
#include "base/diagnostics/stored_report.h"

begin_suite(GobContainerTest);

test_case(Empty) {
    gorc::io::native_file file("src/content_test/vfs/gob/empty.gob");
    gorc::content::vfs::gob::gob_container cnt("src/content_test/vfs/gob/empty.gob", file);

    assert_eq(cnt.file_count(), 0UL);
}

test_case(OneFile) {
    gorc::io::native_file file("src/content_test/vfs/gob/onefile.gob");
    gorc::content::vfs::gob::gob_container cnt("src/content_test/vfs/gob/onefile.gob", file);

    assert_eq(cnt.file_count(), 1UL);

    const gorc::content::vfs::virtual_file& vf = cnt.get_virtual_file(0);
    assert_eq(vf.get_filename(), "test.txt");
    std::unique_ptr<gorc::io::read_only_file> test_txt = vf.open();

    gorc::diagnostics::stored_report report;

    gorc::text::source src("test.txt", *test_txt);
    gorc::text::tokenizer tok(src, report);

    assert_eq(tok.get_number<int>(), 1);
    tok.assert_end_of_file();
}

test_case(ThreeFiles) {
    gorc::io::native_file file("src/content_test/vfs/gob/threefiles.gob");
    gorc::content::vfs::gob::gob_container cnt("src/content_test/vfs/gob/threefiles.gob", file);

    assert_eq(cnt.file_count(), 3UL);

    const gorc::content::vfs::virtual_file& vf1 = cnt.get_virtual_file(0);
    assert_eq(vf1.get_filename(), "first.txt");
    std::unique_ptr<gorc::io::read_only_file> test_txt1 = vf1.open();

    gorc::diagnostics::stored_report report;

    gorc::text::source src1("first.txt", *test_txt1);
    gorc::text::tokenizer tok1(src1, report);

    assert_eq(tok1.get_number<int>(), 1);
    tok1.assert_end_of_file();

    const gorc::content::vfs::virtual_file& vf2 = cnt.get_virtual_file(1);
    assert_eq(vf2.get_filename(), "second.txt");
    std::unique_ptr<gorc::io::read_only_file> test_txt2 = vf2.open();

    gorc::text::source src2("second.txt", *test_txt2);
    gorc::text::tokenizer tok2(src2, report);

    assert_eq(tok2.get_number<int>(), 2);
    tok2.assert_end_of_file();

    const gorc::content::vfs::virtual_file& vf3 = cnt.get_virtual_file(2);
    assert_eq(vf3.get_filename(), "third.txt");
    std::unique_ptr<gorc::io::read_only_file> test_txt3 = vf3.open();

    gorc::text::source src3("third.txt", *test_txt3);
    gorc::text::tokenizer tok3(src3, report);

    assert_eq(tok3.get_number<int>(), 3);
    tok3.assert_end_of_file();
}

test_case(Structured) {
    gorc::io::native_file file("src/content_test/vfs/gob/structured.gob");
    gorc::content::vfs::gob::gob_container cnt("src/content_test/vfs/gob/structured.gob", file);

    assert_eq(cnt.file_count(), 1UL);

    const gorc::content::vfs::virtual_file& vf = cnt.get_virtual_file(0);
    assert_eq(vf.get_filename().generic_string(), boost::filesystem::path("dir/test.txt").generic_string());
    std::unique_ptr<gorc::io::read_only_file> test_txt = vf.open();

    gorc::diagnostics::stored_report report;

    gorc::text::source src("dir/test.txt", *test_txt);
    gorc::text::tokenizer tok(src, report);

    assert_eq(tok.get_number<int>(), 3);
    tok.assert_end_of_file();
}

end_suite(GobContainerTest);
