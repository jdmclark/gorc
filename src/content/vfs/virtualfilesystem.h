#pragma once

#include "framework/io/readonlyfile.h"
#include "framework/diagnostics/report.h"
#include "virtualfile.h"
#include "container.h"
#include "content/vfs/episode/episode.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace Gorc {
namespace Content {
namespace VFS {

typedef std::unordered_map<std::string, const VirtualFile*> vfsmap;

class VirtualFileSystem {
private:
	class VfsEpisode {
	private:
		std::unique_ptr<Container> container;
		vfsmap FileMap;
		std::unique_ptr<Episode::Episode> Episode;

	public:
		VfsEpisode(IO::ReadOnlyFile& file, std::unique_ptr<Container>& container, Diagnostics::Report& report);

		const Episode::Episode& GetEpisode() const;
		vfsmap::const_iterator find(const std::string& path) const;
		vfsmap::const_iterator begin() const;
		vfsmap::const_iterator end() const;
	};

	const boost::filesystem::path RestrictedPath;
	const boost::filesystem::path ResourcePath;
	const boost::filesystem::path GamePath;
	bool hasGamePath;

	std::vector<std::unique_ptr<VfsEpisode>> Episodes;
	VfsEpisode* currentEpisode;

	vfsmap RestrictedFileMap;
	vfsmap ResourceFileMap;
	vfsmap GameFileMap;

	std::vector<std::unique_ptr<Container>> containers;

	void loadGob(const boost::filesystem::path& gobFilePath, vfsmap& map, Diagnostics::Report& report);
	void loadGobs(const boost::filesystem::path& basepath, vfsmap& map, Diagnostics::Report& report);

	std::unique_ptr<IO::ReadOnlyFile> findInGobs(const boost::filesystem::path& path,
			const boost::filesystem::path& basePath, const std::string& generic_string, const vfsmap& map) const;

public:
	VirtualFileSystem(const boost::filesystem::path& RestrictedPath, const boost::filesystem::path& ResourcePath,
			const boost::filesystem::path& EpisodePath, Diagnostics::Report& report);
	VirtualFileSystem(const boost::filesystem::path& RestrictedPath, const boost::filesystem::path& ResourcePath,
			const boost::filesystem::path& EpisodePath, const boost::filesystem::path& GamePath, Diagnostics::Report& report);

	std::unique_ptr<IO::ReadOnlyFile> Open(const boost::filesystem::path& path) const;

	inline size_t GetEpisodeCount() const {
		return Episodes.size();
	}

	inline const Episode::Episode& GetEpisode(size_t index) const {
		return Episodes[index]->GetEpisode();
	}

	inline void SetEpisode(size_t index) {
		currentEpisode = Episodes[index].get();
	}

	inline void ClearEpisode() {
		currentEpisode = nullptr;
	}
};

}
}
}
