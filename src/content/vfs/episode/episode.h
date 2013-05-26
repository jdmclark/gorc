#pragma once

#include "framework/io/readonlyfile.h"
#include "framework/diagnostics/report.h"
#include "entry.h"
#include "episodetype.h"
#include "content/vfs/container.h"
#include <string>
#include <unordered_map>

namespace Gorc {
namespace Content {
namespace VFS {
namespace Episode {

class Episode {
private:
	std::unordered_map<int, size_t> entryMap;
	std::vector<Entry> entries;

	std::string EpisodeName;
	EpisodeType Type;

public:
	Episode(IO::ReadOnlyFile& file, Diagnostics::Report& report);

	inline const std::string& GetEpisodeName() const {
		return EpisodeName;
	}

	inline EpisodeType GetType() const {
		return Type;
	}

	inline auto begin() const -> decltype(entries.begin()) {
		return entries.begin();
	}

	inline auto end() const -> decltype(entries.end()) {
		return entries.end();
	}

	const Entry& GetEntry(int LineNumber) const;
};

}
}
}
}
