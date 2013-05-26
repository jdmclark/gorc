#include <nullunit/nullunit.h>
#include <algorithm>
#include "content/vfs/virtualfilesystem.h"
#include "framework/text/tokenizer.h"
#include "framework/diagnostics/storedreport.h"
#include "content/vfs/episode/episode.h"

class VirtualFileSystemTestFixture : public NullUnit::Fixture {
protected:
	Gorc::Diagnostics::StoredReport report;

public:
	int GetIntFromFile(Gorc::Content::VFS::VirtualFileSystem& vfs, const boost::filesystem::path& filename) {
		std::unique_ptr<Gorc::IO::ReadOnlyFile> file = vfs.Open(filename);
		Gorc::Diagnostics::StoredReport report;
		Gorc::Text::Source src(*file);
		Gorc::Text::Tokenizer tok(src, report);

		int result = tok.GetNumber<int>();
		tok.AssertEndOfFile();

		return result;
	}
};

BeginSuiteFixture(VirtualFileSystemTest, VirtualFileSystemTestFixture);

Case(Basic) {
	Gorc::Content::VFS::VirtualFileSystem vfs("test/vfs/restricted", "test/vfs/resource", "test/vfs/episode", report);

	Test_Assert_Eq(GetIntFromFile(vfs, "asset.txt"), 1);
	Test_Assert_Eq(GetIntFromFile(vfs, "overridden.txt"), 3);
	Test_Assert_Eq(GetIntFromFile(vfs, "modded.txt"), 1);
}

Case(Mod1) {
	Gorc::Content::VFS::VirtualFileSystem vfs("test/vfs/restricted", "test/vfs/resource", "test/vfs/episode",
		"test/vfs/mod1", report);

	Test_Assert_Eq(GetIntFromFile(vfs, "asset.txt"), 1);
	Test_Assert_Eq(GetIntFromFile(vfs, "overridden.txt"), 3);
	Test_Assert_Eq(GetIntFromFile(vfs, "modded.txt"), 2);
}

Case(Mod2) {
	Gorc::Content::VFS::VirtualFileSystem vfs("test/vfs/restricted", "test/vfs/resource", "test/vfs/episode",
		"test/vfs/mod2", report);

	Test_Assert_Eq(GetIntFromFile(vfs, "asset.txt"), 1);
	Test_Assert_Eq(GetIntFromFile(vfs, "overridden.txt"), 3);
	Test_Assert_Eq(GetIntFromFile(vfs, "modded.txt"), 4);
}

Case(Episode) {
	Gorc::Content::VFS::VirtualFileSystem vfs("test/vfs/restricted", "test/vfs/resource", "test/vfs/episode", report);

	Test_Assert_Eq(vfs.GetEpisodeCount(), 2);

	// "Test Episode 1" and "Test Episode 2" should both appear, although not necessarily in a particular order.
	// Store the names and sort them.
	std::vector<std::string> episode_names = { vfs.GetEpisode(0).GetEpisodeName(), vfs.GetEpisode(1).GetEpisodeName() };
	std::sort(episode_names.begin(), episode_names.end());

	std::vector<std::string> episode_name_comparison = { "Test Episode 1", "Test Episode 2" };

	Test_Assert_Eq(episode_names, episode_name_comparison);
}

EndSuite(VirtualFileSystemTest);
