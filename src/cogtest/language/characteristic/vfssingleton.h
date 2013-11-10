#pragma once

#include "framework/diagnostics/stream_report.h"
#include "content/vfs/virtual_filesystem.h"

class VfsSingleton {
private:
	VfsSingleton();
	static VfsSingleton inst;
	gorc::diagnostics::stream_report report;
	gorc::content::vfs::virtual_filesystem vfs;

public:
	inline static const gorc::content::filesystem& get() {
		return inst.vfs;
	}

	static void SetEpisode(const std::string& name);
};
