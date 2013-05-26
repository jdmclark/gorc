#include <nullunit/nullunit.h>
#include "content/vfs/gob/gobcontainer.h"
#include "framework/io/nativefile.h"
#include "framework/text/source.h"
#include "framework/text/tokenizer.h"
#include "framework/diagnostics/storedreport.h"

BeginSuite(GobContainerTest);

Case(Empty) {
	Gorc::IO::NativeFile file("test/gob/empty.gob");
	Gorc::Content::VFS::Gob::GobContainer cnt(file);

	Test_Assert_Eq(cnt.FileCount(), 0);
}

Case(OneFile) {
	Gorc::IO::NativeFile file("test/gob/onefile.gob");
	Gorc::Content::VFS::Gob::GobContainer cnt(file);

	Test_Assert_Eq(cnt.FileCount(), 1);

	const Gorc::Content::VFS::VirtualFile& vf = cnt.GetVirtualFile(0);
	Test_Assert_Eq(vf.GetFilename(), "test.txt");
	std::unique_ptr<Gorc::IO::ReadOnlyFile> test_txt = vf.Open();

	Gorc::Diagnostics::StoredReport report;

	Gorc::Text::Source src(*test_txt);
	Gorc::Text::Tokenizer tok(src, report);

	Test_Assert_Eq(tok.GetNumber<int>(), 1);
	tok.AssertEndOfFile();
}

Case(ThreeFiles) {
	Gorc::IO::NativeFile file("test/gob/threefiles.gob");
	Gorc::Content::VFS::Gob::GobContainer cnt(file);

	Test_Assert_Eq(cnt.FileCount(), 3);

	const Gorc::Content::VFS::VirtualFile& vf1 = cnt.GetVirtualFile(0);
	Test_Assert_Eq(vf1.GetFilename(), "first.txt");
	std::unique_ptr<Gorc::IO::ReadOnlyFile> test_txt1 = vf1.Open();

	Gorc::Diagnostics::StoredReport report;

	Gorc::Text::Source src1(*test_txt1);
	Gorc::Text::Tokenizer tok1(src1, report);

	Test_Assert_Eq(tok1.GetNumber<int>(), 1);
	tok1.AssertEndOfFile();

	const Gorc::Content::VFS::VirtualFile& vf2 = cnt.GetVirtualFile(1);
	Test_Assert_Eq(vf2.GetFilename(), "second.txt");
	std::unique_ptr<Gorc::IO::ReadOnlyFile> test_txt2 = vf2.Open();

	Gorc::Text::Source src2(*test_txt2);
	Gorc::Text::Tokenizer tok2(src2, report);

	Test_Assert_Eq(tok2.GetNumber<int>(), 2);
	tok2.AssertEndOfFile();

	const Gorc::Content::VFS::VirtualFile& vf3 = cnt.GetVirtualFile(2);
	Test_Assert_Eq(vf3.GetFilename(), "third.txt");
	std::unique_ptr<Gorc::IO::ReadOnlyFile> test_txt3 = vf3.Open();

	Gorc::Text::Source src3(*test_txt3);
	Gorc::Text::Tokenizer tok3(src3, report);

	Test_Assert_Eq(tok3.GetNumber<int>(), 3);
	tok3.AssertEndOfFile();
}

Case(Structured) {
	Gorc::IO::NativeFile file("test/gob/structured.gob");
	Gorc::Content::VFS::Gob::GobContainer cnt(file);

	Test_Assert_Eq(cnt.FileCount(), 1);

	const Gorc::Content::VFS::VirtualFile& vf = cnt.GetVirtualFile(0);
	Test_Assert_Eq(vf.GetFilename().generic_string(), boost::filesystem::path("dir/test.txt").generic_string());
	std::unique_ptr<Gorc::IO::ReadOnlyFile> test_txt = vf.Open();

	Gorc::Diagnostics::StoredReport report;

	Gorc::Text::Source src(*test_txt);
	Gorc::Text::Tokenizer tok(src, report);

	Test_Assert_Eq(tok.GetNumber<int>(), 3);
	tok.AssertEndOfFile();
}

EndSuite(GobContainerTest);
