#include "virtual_filesystem.h"
#include "content/vfs/episode/episode.h"
#include "content/vfs/gob/gob_container.h"
#include "framework/io/native_file.h"
#include "framework/io/exception.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>

using Gorc::Content::VFS::vfsmap;

Gorc::Content::VFS::VirtualFileSystem::VfsEpisode::VfsEpisode(IO::ReadOnlyFile& file, std::unique_ptr<Container>& cnt, Diagnostics::Report& report)
	: container(std::move(cnt)) {
	Episode = std::unique_ptr<Episode::Episode>(new Episode::Episode(file, report));

	for(size_t i = 0; i < container->FileCount(); ++i) {
		const VirtualFile& vf = container->GetVirtualFile(i);
		std::string fn = vf.GetFilename().generic_string();
		std::transform(fn.begin(), fn.end(), fn.begin(), tolower);
		FileMap.insert(std::make_pair(fn, &vf));
	}

	return;
}

const Gorc::Content::VFS::Episode::Episode& Gorc::Content::VFS::VirtualFileSystem::VfsEpisode::GetEpisode() const {
	return *Episode;
}

vfsmap::const_iterator Gorc::Content::VFS::VirtualFileSystem::VfsEpisode::find(const std::string& path) const {
	return FileMap.find(path);
}

vfsmap::const_iterator Gorc::Content::VFS::VirtualFileSystem::VfsEpisode::begin() const {
	return FileMap.begin();
}

vfsmap::const_iterator Gorc::Content::VFS::VirtualFileSystem::VfsEpisode::end() const {
	return FileMap.end();
}

Gorc::Content::VFS::VirtualFileSystem::VirtualFileSystem(const boost::filesystem::path& RestrictedPath, const boost::filesystem::path& ResourcePath,
		const boost::filesystem::path& EpisodePath, Diagnostics::Report& report)
	: RestrictedPath(RestrictedPath), ResourcePath(ResourcePath), hasGamePath(false), currentEpisode(nullptr) {
	loadGobs(RestrictedPath, RestrictedFileMap, report);
	loadGobs(ResourcePath, ResourceFileMap, report);
	loadGobs(EpisodePath, ResourceFileMap, report);
}

Gorc::Content::VFS::VirtualFileSystem::VirtualFileSystem(const boost::filesystem::path& RestrictedPath, const boost::filesystem::path& ResourcePath,
		const boost::filesystem::path& EpisodePath, const boost::filesystem::path& GamePath, Diagnostics::Report& report)
	: RestrictedPath(RestrictedPath), ResourcePath(ResourcePath), GamePath(GamePath), hasGamePath(true), currentEpisode(nullptr) {
	loadGobs(RestrictedPath, RestrictedFileMap, report);
	loadGobs(ResourcePath, ResourceFileMap, report);
	loadGobs(EpisodePath, ResourceFileMap, report);
	loadGobs(GamePath, GameFileMap, report);
}

void Gorc::Content::VFS::VirtualFileSystem::loadGob(const boost::filesystem::path& gobFilePath,	vfsmap& map, Diagnostics::Report& report) {
	std::unique_ptr<Container> cnt;

	try {
		IO::NativeFile file(gobFilePath, false);
		cnt = std::unique_ptr<Container>(new Gob::GobContainer(file));
	}
	catch(IO::IOException&) {
		report.AddWarning("VirtualFileSystem", "container file corrupt",
				Diagnostics::ErrorLocation(gobFilePath, 0, 0, 0, 0));
		return;
	}

	if(cnt->IsEpisode()) {
		try {
			std::unique_ptr<IO::ReadOnlyFile> file = cnt->GetEpisode().Open();
			Episodes.emplace_back(new VfsEpisode(*file, cnt, report));
		}
		catch(...) {
			report.AddWarning("VirtualFileSystem", "episode file corrupt",
					Diagnostics::ErrorLocation(gobFilePath / "episode.jk", 0, 0, 0, 0));
		}
	}
	else {
		for(size_t i = 0; i < cnt->FileCount(); ++i) {
			const VirtualFile& vf = cnt->GetVirtualFile(i);
			std::string fn = vf.GetFilename().generic_string();
			std::transform(fn.begin(), fn.end(), fn.begin(), tolower);
			map.insert(std::make_pair(fn, &vf));
		}

		containers.push_back(std::move(cnt));
	}
}

void Gorc::Content::VFS::VirtualFileSystem::loadGobs(const boost::filesystem::path& basepath, vfsmap& map, Diagnostics::Report& report) {
	if(boost::filesystem::exists(basepath) && boost::filesystem::is_directory(basepath)) {
		for(boost::filesystem::directory_iterator dir_iter(basepath); dir_iter != boost::filesystem::directory_iterator(); ++dir_iter) {
			if(boost::filesystem::is_regular_file(dir_iter->status())
				&& boost::iequals(dir_iter->path().extension().generic_string(), ".gob")) {
				loadGob(dir_iter->path(), map, report);
			}
		}
	}
}

std::unique_ptr<Gorc::IO::ReadOnlyFile> Gorc::Content::VFS::VirtualFileSystem::findInGobs(const boost::filesystem::path& path,
		const boost::filesystem::path& basePath, const std::string& generic_string, const vfsmap& map) const {
	boost::filesystem::path inBareDirectory = basePath / path;
	if(boost::filesystem::exists(inBareDirectory) && boost::filesystem::is_regular_file(inBareDirectory)) {
		return std::unique_ptr<IO::ReadOnlyFile>(new IO::NativeFile(inBareDirectory));
	}

	auto inGob = map.find(generic_string);
	if(inGob != map.end()) {
		return inGob->second->Open();
	}

	return std::unique_ptr<IO::ReadOnlyFile>();
}

std::unique_ptr<Gorc::IO::ReadOnlyFile> Gorc::Content::VFS::VirtualFileSystem::Open(const boost::filesystem::path& path) const {
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
			return epPath->second->Open();
		}
	}

	// Try from mod files.
	if(hasGamePath) {
		auto gameFile = findInGobs(npath, GamePath, generic_string, GameFileMap);
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
	throw IO::FileNotFoundException();
}
