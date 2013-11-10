#include "vfssingleton.h"
#include <boost/format.hpp>

VfsSingleton VfsSingleton::inst;

VfsSingleton::VfsSingleton()
	: report(std::cout), vfs("game/restricted", "game/resource", "game/episode", report) {
	return;
}

void VfsSingleton::SetEpisode(const std::string& name) {
	gorc::content::vfs::virtual_filesystem& vfs = inst.vfs;

	// Find episode
	for(size_t i = 0; i < vfs.get_episode_count(); ++i) {
		const auto& ep = vfs.get_episode(i);
		if(ep.get_episode_name() == name) {
			vfs.set_episode(i);
			return;
		}
	}

	std::cout << boost::str(boost::format("could not find episode %s") % name) << std::endl;
	throw std::exception();
}
