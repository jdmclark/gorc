#include "test/test.h"
#include "content/vfs/episode/episode.h"
#include "base/io/native_file.h"
#include "base/diagnostics/stored_report.h"
#include "content/vfs/episode/exception.h"
#include "content/vfs/container.h"
#include "base/io/exception.h"
#include "base/text/exception.h"

begin_suite(EpisodeTest);

test_case(PreparedTest) {
    gorc::diagnostics::stored_report report;
    gorc::io::native_file file("src/content_test/vfs/episode/test.jk");
    gorc::content::vfs::episode::episode ep(file, report);

    assert_eq(ep.get_episode_name(), "The Farce Without");
    assert_eq(ep.get_type(), gorc::content::vfs::episode::episode_type::single_player);

    const gorc::content::vfs::episode::entry& entry_21bjerk = ep.get_entry(810);
    assert_eq(entry_21bjerk.disc, 2);
    assert_eq(entry_21bjerk.type, gorc::content::vfs::episode::entry_type::level);
    assert_eq(entry_21bjerk.filename, "21bjerk.jkl");

    const gorc::content::vfs::episode::entry& entry_decide = ep.get_entry(143);
    assert_eq(entry_decide.type, gorc::content::vfs::episode::entry_type::decision);
    assert_eq(entry_decide.goto_a, 200);
    assert_eq(entry_decide.goto_b, 800);
}

end_suite(EpisodeTest);
