#include "entry.h"

gorc::content::vfs::episode::entry::entry(int number, int Disc, int LevelNumber,
		entry_type type, const boost::filesystem::path& Filename, int LightPower, int DarkPower,
		int GotoA, int GotoB)
	: number(number), disc(Disc), level_number(LevelNumber), type(type), filename(Filename),
	  light_power(LightPower), dark_power(DarkPower), goto_a(GotoA), goto_b(GotoB) {
	return;
}
