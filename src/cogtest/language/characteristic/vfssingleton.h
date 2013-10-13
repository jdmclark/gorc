#pragma once

#include "framework/diagnostics/stream_report.h"
#include "content/vfs/virtual_filesystem.h"

class VfsSingleton {
private:
	VfsSingleton();
	static VfsSingleton inst;
	Gorc::Diagnostics::StreamReport report;
	Gorc::Content::VFS::VirtualFileSystem vfs;

public:
	inline static const Gorc::Content::FileSystem& Get() {
		return inst.vfs;
	}

	static void SetEpisode(const std::string& name);
};
