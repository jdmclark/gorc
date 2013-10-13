#pragma once

#include "entry_type.h"
#include <boost/filesystem/path.hpp>

namespace Gorc {
namespace Content {
namespace VFS {
namespace Episode {

class Entry {
public:
	const int Number;
	const int Disc;
	const int LevelNumber;
	const EntryType Type;
	const boost::filesystem::path Filename;
	const int LightPower;
	const int DarkPower;
	const int GotoA;
	const int GotoB;

	Entry(int Number, int Disc, int LevelNumber, EntryType Type,
			const boost::filesystem::path& Filename, int LightPower, int DarkPower,
			int GotoA, int GotoB);
};

}
}
}
}
