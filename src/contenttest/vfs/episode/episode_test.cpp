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
	Gorc::Diagnostics::StoredReport report;
	Gorc::IO::NativeFile file("test/episode/test.jk");
	Gorc::Content::VFS::Episode::Episode ep(file, report);

	Test_Assert_Eq(ep.GetEpisodeName(), "The Farce Without");
	Test_Assert_Eq(ep.GetType(), Gorc::Content::VFS::Episode::EpisodeType::SinglePlayer);

	const Gorc::Content::VFS::Episode::Entry& entry_21bjerk = ep.GetEntry(810);
	Test_Assert_Eq(entry_21bjerk.Disc, 2);
	Test_Assert_Eq(entry_21bjerk.Type, Gorc::Content::VFS::Episode::EntryType::Level);
	Test_Assert_Eq(entry_21bjerk.Filename, "21bjerk.jkl");

	const Gorc::Content::VFS::Episode::Entry& entry_decide = ep.GetEntry(143);
	Test_Assert_Eq(entry_decide.Type, Gorc::Content::VFS::Episode::EntryType::Decision);
	Test_Assert_Eq(entry_decide.GotoA, 200);
	Test_Assert_Eq(entry_decide.GotoB, 800);
}

EndSuite(EpisodeTest);
