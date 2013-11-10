#include <nullunit/nullunit.h>
#include "content/vfs/gob/gob_container.h"
#include "framework/io/native_file.h"
#include "framework/text/source.h"
#include "framework/text/tokenizer.h"
#include "framework/diagnostics/stored_report.h"

BeginSuite(GobContainerTest);

Case(Empty) {
	gorc::io::native_file file("test/gob/empty.gob");
	gorc::content::vfs::gob::gob_container cnt(file);

	Test_Assert_Eq(cnt.file_count(), 0);
}

Case(OneFile) {
	gorc::io::native_file file("test/gob/onefile.gob");
	gorc::content::vfs::gob::gob_container cnt(file);

	Test_Assert_Eq(cnt.file_count(), 1);

	const gorc::content::vfs::virtual_file& vf = cnt.get_virtual_file(0);
	Test_Assert_Eq(vf.get_filename(), "test.txt");
	std::unique_ptr<gorc::io::read_only_file> test_txt = vf.open();

	gorc::diagnostics::stored_report report;

	gorc::text::source src(*test_txt);
	gorc::text::tokenizer tok(src, report);

	Test_Assert_Eq(tok.get_number<int>(), 1);
	tok.assert_end_of_file();
}

Case(ThreeFiles) {
	gorc::io::native_file file("test/gob/threefiles.gob");
	gorc::content::vfs::gob::gob_container cnt(file);

	Test_Assert_Eq(cnt.file_count(), 3);

	const gorc::content::vfs::virtual_file& vf1 = cnt.get_virtual_file(0);
	Test_Assert_Eq(vf1.get_filename(), "first.txt");
	std::unique_ptr<gorc::io::read_only_file> test_txt1 = vf1.open();

	gorc::diagnostics::stored_report report;

	gorc::text::source src1(*test_txt1);
	gorc::text::tokenizer tok1(src1, report);

	Test_Assert_Eq(tok1.get_number<int>(), 1);
	tok1.assert_end_of_file();

	const gorc::content::vfs::virtual_file& vf2 = cnt.get_virtual_file(1);
	Test_Assert_Eq(vf2.get_filename(), "second.txt");
	std::unique_ptr<gorc::io::read_only_file> test_txt2 = vf2.open();

	gorc::text::source src2(*test_txt2);
	gorc::text::tokenizer tok2(src2, report);

	Test_Assert_Eq(tok2.get_number<int>(), 2);
	tok2.assert_end_of_file();

	const gorc::content::vfs::virtual_file& vf3 = cnt.get_virtual_file(2);
	Test_Assert_Eq(vf3.get_filename(), "third.txt");
	std::unique_ptr<gorc::io::read_only_file> test_txt3 = vf3.open();

	gorc::text::source src3(*test_txt3);
	gorc::text::tokenizer tok3(src3, report);

	Test_Assert_Eq(tok3.get_number<int>(), 3);
	tok3.assert_end_of_file();
}

Case(Structured) {
	gorc::io::native_file file("test/gob/structured.gob");
	gorc::content::vfs::gob::gob_container cnt(file);

	Test_Assert_Eq(cnt.file_count(), 1);

	const gorc::content::vfs::virtual_file& vf = cnt.get_virtual_file(0);
	Test_Assert_Eq(vf.get_filename().generic_string(), boost::filesystem::path("dir/test.txt").generic_string());
	std::unique_ptr<gorc::io::read_only_file> test_txt = vf.open();

	gorc::diagnostics::stored_report report;

	gorc::text::source src(*test_txt);
	gorc::text::tokenizer tok(src, report);

	Test_Assert_Eq(tok.get_number<int>(), 3);
	tok.assert_end_of_file();
}

EndSuite(GobContainerTest);
