#include "framework/diagnostics/stream_report.h"
#include "content/vfs/virtual_filesystem.h"
#include <iostream>
#include <string>
#include <boost/algorithm/string/predicate.hpp>

int main(int argc, char** argv) {
    gorc::diagnostics::stream_report report(std::cout);
    gorc::content::vfs::virtual_filesystem FileSystem("game/restricted",
            "game/resource", "game/episode", report);
    if(argc < 2 || argc > 3) {
        std::cout << "Usage:" << std::endl;
        std::cout << "\tungob \"Episode Name\" \"path/to/file\"" << std::endl;
        std::cout << "\tungob \"Episode Name\"" << std::endl;
        return 0;
    }

    for(size_t i = 0; i < FileSystem.get_episode_count(); ++i) {
        if(boost::iequals(FileSystem.get_episode(i).get_episode_name(), argv[1])) {
            FileSystem.set_episode(i);
        }
    }

    if(argc == 3 && argv[2]) {
        auto file = FileSystem.open(argv[2]);

        std::string buffer(file->get_size(), ' ');
        file->read(&buffer[0], file->get_size());

        std::cout << buffer << std::endl;
    }
    else {
        FileSystem.list(std::cout);
    }

    return 0;
}
