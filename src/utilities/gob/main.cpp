#include "program/program.hpp"
#include "vfs/gob_virtual_container.hpp"
#include "vfs/jk_virtual_file_system.hpp"
#include "io/std_output_stream.hpp"
#include <iostream>
#include <iomanip>
#include <map>

namespace gorc {

    class gob_program : public program {
    private:
        // Archive modes
        std::string archive_file;

        bool jk_mode = false;
        std::string jk_game;
        std::string jk_episode;
        std::string jk_resource = "resource";

        // Sub-commands
        bool do_list = false;
        std::string extract_file;

    public:
        virtual void create_options(options &opts) override
        {
            // Archive modes
            opts.insert(make_value_option("file", archive_file));
            opts.insert(make_switch_option("jk", jk_mode));

            opts.emplace_constraint<gorc::mutual_exclusion>(
                    std::vector<std::string> { "file", "jk" },
                    /* min set */ 1,
                    /* max set */ 1);

            opts.insert(make_value_option("game", jk_game));
            opts.emplace_constraint<gorc::dependent_option>("game", "jk");

            opts.insert(make_value_option("episode", jk_episode));
            opts.emplace_constraint<gorc::dependent_option>("episode", "jk");

            opts.insert(make_value_option("resource", jk_resource));
            opts.emplace_constraint<gorc::dependent_option>("resource", "jk");

            // Sub-command modes
            opts.insert(make_switch_option("list", do_list));
            opts.insert(make_value_option("extract", extract_file));
            opts.emplace_constraint<gorc::mutual_exclusion>(
                    std::vector<std::string> { "list", "extract" },
                    /* min set */ 1,
                    /* max set */ 1);
        }

        virtual int main() override
        {
            if(jk_mode) {
                // Handle JK VFS mode
                return jk_vfs_main();
            }
            else if(!archive_file.empty()) {
                // Handle single archive
                return single_archive_main();
            }

            // LCOV_EXCL_START
            // Not possible - guards against future changes
            LOG_FATAL("unhandled option");
            // LCOV_EXCL_STOP
        }

        int jk_vfs_main()
        {
            std::unique_ptr<jk_virtual_file_system> vfs;
            if(jk_game.empty()) {
                vfs = std::make_unique<jk_virtual_file_system>(jk_resource);
            }
            else {
                vfs = std::make_unique<jk_virtual_file_system>(jk_resource, jk_game);
            }

            std::unique_ptr<virtual_container> episode_gob;
            if(!jk_episode.empty()) {
                episode_gob = std::make_unique<gob_virtual_container>(jk_episode);
                vfs->set_current_episode(*episode_gob);
            }

            // Handle sub-commands
            if(!extract_file.empty()) {
                return jk_vfs_extract_file(*vfs);
            }
            else {
                // Default: list files
                return jk_vfs_list(*vfs);
            }
        }

        int single_archive_main()
        {
            gob_virtual_container gob(archive_file);

            if(!extract_file.empty()) {
                return single_archive_extract_file(gob);
            }
            else {
                // Default: list files
                return single_archive_list(gob);
            }
        }

        int jk_vfs_list(jk_virtual_file_system const &vfs)
        {
            auto vfs_map = vfs.list_files();
            size_t num_files = 0;
            for(auto const &f : vfs_map) {
                std::cout << f.first << " => " << f.second << std::endl;
                ++num_files;
            }
            std::cout << "--------------------" << std::endl;
            std::cout << "Files: " << num_files << std::endl;
            return EXIT_SUCCESS;
        }

        int single_archive_list(gob_virtual_container const &gob)
        {
            for(size_t i = 0; i < gob.size(); ++i) {
                auto const &gf = gob.get_file(i);

                std::cout << i + 1
                          << ": "
                          << gf.name.generic_string()
                          << std::endl;
                std::cout << "    Size:   "
                          << std::setw(10)
                          << gf.chunk_length
                          << std::endl;
                std::cout << "    Offset: "
                          << std::setw(10)
                          << gf.chunk_offset
                          << std::endl;
            }

            std::cout << "Total: " << gob.size() << std::endl;

            return EXIT_SUCCESS;
        }

        int jk_vfs_extract_file(virtual_file_system const &vfs)
        {
            // Transform input filename to lowercase
            std::transform(extract_file.begin(),
                           extract_file.end(),
                           extract_file.begin(),
                           tolower);
            path p = extract_file;

            // Locate file in VFS
            auto f = vfs.open(p);

            // Print
            std_output_stream out_stream;
            out_stream.reopen_as_binary();
            f->copy_to(out_stream);

            return EXIT_SUCCESS;
        }

        int single_archive_extract_file(gob_virtual_container const &gob)
        {
            // Transform input filename to lowercase
            std::transform(extract_file.begin(),
                           extract_file.end(),
                           extract_file.begin(),
                           tolower);
            path p = extract_file;

            // Locate file in archive
            for(auto const &file : gob) {
                if(file.name == p) {
                    std_output_stream out_stream;
                    out_stream.reopen_as_binary();
                    auto input_file = file.open();
                    input_file->copy_to(out_stream);
                    return EXIT_SUCCESS;
                }
            }

            LOG_FATAL(format("file %s not in archive") % extract_file);
        }
    };

}

MAKE_MAIN(gorc::gob_program)
