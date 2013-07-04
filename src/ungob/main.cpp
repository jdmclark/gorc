#include "framework/diagnostics/streamreport.h"
#include "content/vfs/virtualfilesystem.h"
#include <iostream>
#include <string>
#include <boost/algorithm/string/predicate.hpp>

int main(int argc, char** argv) {
	Gorc::Diagnostics::StreamReport Report(std::cout);
	Gorc::Content::VFS::VirtualFileSystem FileSystem("game/restricted",
			"game/resource", "game/episode", Report);
	if(argc != 3) {
		std::cout << "Usage: ungob \"Episode Name\" \"path/to/file\"" << std::endl;
		return 0;
	}

	for(size_t i = 0; i < FileSystem.GetEpisodeCount(); ++i) {
		if(boost::iequals(FileSystem.GetEpisode(i).GetEpisodeName(), argv[1])) {
			FileSystem.SetEpisode(i);
		}
	}

	auto file = FileSystem.Open(argv[2]);

	std::string buffer(file->GetSize(), ' ');
	file->Read(&buffer[0], file->GetSize());

	std::cout << buffer << std::endl;

	return 0;
}
