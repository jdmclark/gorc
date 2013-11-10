#include "virtual_filesystem.h"
#include "content/vfs/episode/episode.h"
#include "content/vfs/gob/gob_container.h"
#include "framework/io/native_file.h"
#include "framework/io/exception.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>

using gorc::content::vfs::vfsmap;

gorc::content::vfs::virtual_filesystem::vfs_episode::vfs_episode(io::read_only_file& file, std::unique_ptr<class container>& cnt, diagnostics::report& report)
	: container(std::move(cnt)) {
	Episode = std::unique_ptr<episode::episode>(new episode::episode(file, report));

	for(size_t i = 0; i < container->file_count(); ++i) {
		const virtual_file& vf = container->get_virtual_file(i);
		std::string fn = vf.get_filename().generic_string();
		std::transform(fn.begin(), fn.end(), fn.begin(), tolower);
		FileMap.insert(std::make_pair(fn, &vf));
	}

	return;
}

const gorc::content::vfs::episode::episode& gorc::content::vfs::virtual_filesystem::vfs_episode::get_episode() const {
	return *Episode;
}

vfsmap::const_iterator gorc::content::vfs::virtual_filesystem::vfs_episode::find(const std::string& path) const {
	return FileMap.find(path);
}

vfsmap::const_iterator gorc::content::vfs::virtual_filesystem::vfs_episode::begin() const {
	return FileMap.begin();
}

vfsmap::const_iterator gorc::content::vfs::virtual_filesystem::vfs_episode::end() const {
	return FileMap.end();
}

gorc::content::vfs::virtual_filesystem::virtual_filesystem(const boost::filesystem::path& RestrictedPath, const boost::filesystem::path& ResourcePath,
		const boost::filesystem::path& EpisodePath, diagnostics::report& report)
	: RestrictedPath(RestrictedPath), ResourcePath(ResourcePath), hasgamePath(false), currentEpisode(nullptr) {
	loadGobs(RestrictedPath, RestrictedFileMap, report);
	loadGobs(ResourcePath, ResourceFileMap, report);
	loadGobs(EpisodePath, ResourceFileMap, report);
}

gorc::content::vfs::virtual_filesystem::virtual_filesystem(const boost::filesystem::path& RestrictedPath, const boost::filesystem::path& ResourcePath,
		const boost::filesystem::path& EpisodePath, const boost::filesystem::path& gamePath, diagnostics::report& report)
	: RestrictedPath(RestrictedPath), ResourcePath(ResourcePath), gamePath(gamePath), hasgamePath(true), currentEpisode(nullptr) {
	loadGobs(RestrictedPath, RestrictedFileMap, report);
	loadGobs(ResourcePath, ResourceFileMap, report);
	loadGobs(EpisodePath, ResourceFileMap, report);
	loadGobs(gamePath, gameFileMap, report);
}

void gorc::content::vfs::virtual_filesystem::loadGob(const boost::filesystem::path& gobFilePath,	vfsmap& map, diagnostics::report& report) {
	std::unique_ptr<container> cnt;

	try {
		io::native_file file(gobFilePath, false);
		cnt = std::unique_ptr<container>(new gob::gob_container(file));
	}
	catch(io::io_exception&) {
		report.add_warning("VirtualFileSystem", "container file corrupt",
				diagnostics::error_location(gobFilePath, 0, 0, 0, 0));
		return;
	}

	if(cnt->is_episode()) {
		try {
			std::unique_ptr<io::read_only_file> file = cnt->get_episode().open();
			episodes.emplace_back(new vfs_episode(*file, cnt, report));
		}
		catch(...) {
			report.add_warning("VirtualFileSystem", "episode file corrupt",
					diagnostics::error_location(gobFilePath / "episode.jk", 0, 0, 0, 0));
		}
	}
	else {
		for(size_t i = 0; i < cnt->file_count(); ++i) {
			const virtual_file& vf = cnt->get_virtual_file(i);
			std::string fn = vf.get_filename().generic_string();
			std::transform(fn.begin(), fn.end(), fn.begin(), tolower);
			map.insert(std::make_pair(fn, &vf));
		}

		containers.push_back(std::move(cnt));
	}
}

void gorc::content::vfs::virtual_filesystem::loadGobs(const boost::filesystem::path& basepath, vfsmap& map, diagnostics::report& report) {
	if(boost::filesystem::exists(basepath) && boost::filesystem::is_directory(basepath)) {
		for(boost::filesystem::directory_iterator dir_iter(basepath); dir_iter != boost::filesystem::directory_iterator(); ++dir_iter) {
			if(boost::filesystem::is_regular_file(dir_iter->status())
				&& boost::iequals(dir_iter->path().extension().generic_string(), ".gob")) {
				loadGob(dir_iter->path(), map, report);
			}
		}
	}
}

std::unique_ptr<gorc::io::read_only_file> gorc::content::vfs::virtual_filesystem::findInGobs(const boost::filesystem::path& path,
		const boost::filesystem::path& basePath, const std::string& generic_string, const vfsmap& map) const {
	boost::filesystem::path inBareDirectory = basePath / path;
	if(boost::filesystem::exists(inBareDirectory) && boost::filesystem::is_regular_file(inBareDirectory)) {
		return std::unique_ptr<io::read_only_file>(new io::native_file(inBareDirectory));
	}

	auto inGob = map.find(generic_string);
	if(inGob != map.end()) {
		return inGob->second->open();
	}

	return std::unique_ptr<io::read_only_file>();
}

std::unique_ptr<gorc::io::read_only_file> gorc::content::vfs::virtual_filesystem::open(const boost::filesystem::path& path) const {
	std::string generic_string = path.generic_string();
	std::transform(generic_string.begin(), generic_string.end(), generic_string.begin(), tolower);
	boost::filesystem::path npath(generic_string);

	// Restricted files have the highest priority.
	// These assets are internal to the engine recreation and should not be replaced.
	auto restrictedFile = findInGobs(npath, RestrictedPath, generic_string, RestrictedFileMap);
	if(restrictedFile) {
		return restrictedFile;
	}

	// Try from episode files.
	if(currentEpisode != nullptr) {
		auto epPath = currentEpisode->find(generic_string);
		if(epPath != currentEpisode->end()) {
			return epPath->second->open();
		}
	}

	// Try from mod files.
	if(hasgamePath) {
		auto gameFile = findInGobs(npath, gamePath, generic_string, gameFileMap);
		if(gameFile) {
			return gameFile;
		}
	}

	// Try from resource files.
	auto resourceFile = findInGobs(npath, ResourcePath, generic_string, ResourceFileMap);
	if(resourceFile) {
		return resourceFile;
	}

	// Not found.
	throw io::file_not_found_exception();
}
