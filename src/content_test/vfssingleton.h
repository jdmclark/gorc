#pragma once

#include "base/diagnostics/stream_report.h"
#include "content/vfs/virtual_file_system.h"

class VfsSingleton {
private:
    VfsSingleton();
    static VfsSingleton inst;
    gorc::diagnostics::stream_report report;
    gorc::content::vfs::virtual_file_system vfs;

public:
    inline static const gorc::io::file_system& get() {
        return inst.vfs;
    }

    static void SetEpisode(const std::string& name);
};
