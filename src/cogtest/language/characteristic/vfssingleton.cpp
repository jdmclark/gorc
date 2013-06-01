#include "vfssingleton.h"
#include <boost/format.hpp>

VfsSingleton VfsSingleton::inst;

VfsSingleton::VfsSingleton()
	: report(std::cout), vfs("game/restricted", "game/resource", "game/episode", report) {
	return;
}

void VfsSingleton::SetEpisode(const std::string& name) {
	Gorc::Content::VFS::VirtualFileSystem& vfs = inst.vfs;

	// Find episode
	for(size_t i = 0; i < vfs.GetEpisodeCount(); ++i) {
		const auto& ep = vfs.GetEpisode(i);
		if(ep.GetEpisodeName() == name) {
			vfs.SetEpisode(i);
			return;
		}
	}

	std::cout << boost::str(boost::format("could not find episode %s") % name) << std::endl;
	throw std::exception();
}
