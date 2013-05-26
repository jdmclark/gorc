#include "entry.h"

Gorc::Content::VFS::Episode::Entry::Entry(int Number, int Disc, int LevelNumber,
		EntryType Type, const boost::filesystem::path& Filename, int LightPower, int DarkPower,
		int GotoA, int GotoB)
	: Number(Number), Disc(Disc), LevelNumber(LevelNumber), Type(Type), Filename(Filename),
	  LightPower(LightPower), DarkPower(DarkPower), GotoA(GotoA), GotoB(GotoB) {
	return;
}
