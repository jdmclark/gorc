#include "test/test.h"
#include <algorithm>
#include "content/vfs/virtual_filesystem.h"
#include "framework/text/tokenizer.h"
#include "framework/diagnostics/stored_report.h"
#include "content/vfs/episode/episode.h"

class VirtualFileSystemTestFixture : public test::fixture {
protected:
    gorc::diagnostics::stored_report report;

public:
    int GetIntFromFile(gorc::content::vfs::virtual_filesystem& vfs, const boost::filesystem::path& filename) {
        std::unique_ptr<gorc::io::read_only_file> file = vfs.open(filename);
        gorc::diagnostics::stored_report report;
        gorc::text::source src(*file);
        gorc::text::tokenizer tok(src, report);

        int result = tok.get_number<int>();
        tok.assert_end_of_file();

        return result;
    }
};

begin_suite_fixture(VirtualFileSystemTest, VirtualFileSystemTestFixture);

test_case(Basic) {
    gorc::content::vfs::virtual_filesystem vfs("test/vfs/restricted", "test/vfs/resource", "test/vfs/episode", report);

    assert_eq(GetIntFromFile(vfs, "asset.txt"), 1);
    assert_eq(GetIntFromFile(vfs, "overridden.txt"), 3);
    assert_eq(GetIntFromFile(vfs, "modded.txt"), 1);
}

test_case(Mod1) {
    gorc::content::vfs::virtual_filesystem vfs("test/vfs/restricted", "test/vfs/resource", "test/vfs/episode",
        "test/vfs/mod1", report);

    assert_eq(GetIntFromFile(vfs, "asset.txt"), 1);
    assert_eq(GetIntFromFile(vfs, "overridden.txt"), 3);
    assert_eq(GetIntFromFile(vfs, "modded.txt"), 2);
}

test_case(Mod2) {
    gorc::content::vfs::virtual_filesystem vfs("test/vfs/restricted", "test/vfs/resource", "test/vfs/episode",
        "test/vfs/mod2", report);

    assert_eq(GetIntFromFile(vfs, "asset.txt"), 1);
    assert_eq(GetIntFromFile(vfs, "overridden.txt"), 3);
    assert_eq(GetIntFromFile(vfs, "modded.txt"), 4);
}

test_case(Episode) {
    gorc::content::vfs::virtual_filesystem vfs("test/vfs/restricted", "test/vfs/resource", "test/vfs/episode", report);

    assert_eq(vfs.get_episode_count(), 2);

    // "Test Episode 1" and "Test Episode 2" should both appear, although not necessarily in a particular order.
    // Store the names and sort them.
    std::vector<std::string> episode_names = { vfs.get_episode(0).get_episode_name(), vfs.get_episode(1).get_episode_name() };
    std::sort(episode_names.begin(), episode_names.end());

    std::vector<std::string> episode_name_comparison = { "Test Episode 1", "Test Episode 2" };

    assert_eq(episode_names, episode_name_comparison);
}

end_suite(VirtualFileSystemTest);
