#include <nullunit/nullunit.h>
#include "content/vfs/episode/episode.h"
#include "framework/io/native_file.h"
#include "framework/diagnostics/stored_report.h"
#include "content/vfs/episode/exception.h"
#include "content/vfs/container.h"
#include "framework/io/exception.h"
#include "framework/text/exception.h"

BeginSuite(EpisodeTest);

Case(PreparedTest)
{
	gorc::diagnostics::stored_report report;
	gorc::io::native_file file("test/episode/test.jk");
	gorc::content::vfs::episode::episode ep(file, report);

	Test_Assert_Eq(ep.get_episode_name(), "The Farce Without");
	Test_Assert_Eq(ep.get_type(), gorc::content::vfs::episode::episode_type::single_player);

	const gorc::content::vfs::episode::entry& entry_21bjerk = ep.get_entry(810);
	Test_Assert_Eq(entry_21bjerk.disc, 2);
	Test_Assert_Eq(entry_21bjerk.type, gorc::content::vfs::episode::entry_type::level);
	Test_Assert_Eq(entry_21bjerk.filename, "21bjerk.jkl");

	const gorc::content::vfs::episode::entry& entry_decide = ep.get_entry(143);
	Test_Assert_Eq(entry_decide.type, gorc::content::vfs::episode::entry_type::decision);
	Test_Assert_Eq(entry_decide.goto_a, 200);
	Test_Assert_Eq(entry_decide.goto_b, 800);
}

EndSuite(EpisodeTest);
