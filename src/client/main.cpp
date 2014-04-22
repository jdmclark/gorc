#include <iostream>

#include "application.h"
#include "base/diagnostics/stream_report.h"
#include "content/vfs/virtual_file_system.h"

int main(int argc, char** argv) {
    if(argc != 3) {
        std::cout << "Usage: game \"Episode Name\" levelfilename.jkl" << std::endl;
        return 0;
    }

    std::string episode_name = argv[1];
    std::string levelfile_name = argv[2];

    gorc::diagnostics::stream_report report(std::cout);
    gorc::content::vfs::virtual_file_system fs("game/restricted", "game/resource", "game/episode", report);
    gorc::client::application app(fs, report, episode_name, levelfile_name);
    app.run();
    return 0;
}
