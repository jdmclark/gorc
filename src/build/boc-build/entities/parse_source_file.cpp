#include "parse_source_file.hpp"
#include "log/log.hpp"
#include "utility/range.hpp"
#include <fstream>
#include <string>
#include <regex>
#include <boost/filesystem.hpp>

namespace {

    std::regex include_regex("^#[ \t]*include[ \t]\"([^\"]+)\"",
                             std::regex::optimize);

}

gorc::maybe<std::set<gorc::path>> gorc::get_source_file_headers(path const &p,
                                                                std::vector<path> const &search)
{
    std::ifstream ifs;
    ifs.open(p.native());

    if(ifs.fail()) {
        LOG_ERROR(format("Cannot open source file '%s' for dependency analysis") %
                  p.generic_string());
        return nothing;
    }

    std::set<gorc::path> deps;
    std::string curr_line;
    while(true) {
        std::getline(ifs, curr_line);
        if(!ifs.good()) {
            break;
        }

        for(auto const &include_match : make_range(std::sregex_iterator(curr_line.begin(),
                                                                        curr_line.end(),
                                                                        include_regex),
                                                   std::sregex_iterator())) {
            auto header = include_match[1].str();

            // Search for header relative to current file
            auto rel_path = p.parent_path() / header;
            if(boost::filesystem::is_regular_file(rel_path)) {
                deps.insert(rel_path.normalize());
                continue;
            }

            // Look for header in search path
            bool found = false;
            for(auto const &dir : search) {
                auto dir_path = dir / header;

                if(boost::filesystem::is_regular_file(dir_path)) {
                    deps.insert(dir_path.normalize());
                    found = true;
                    break;
                }
            }

            if(!found) {
                LOG_ERROR(format("Cannot find header file '%s' in search path") %
                          header);
                return nothing;
            }
        }
    }

    return deps;
}
