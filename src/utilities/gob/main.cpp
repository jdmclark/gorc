#include "program/program.hpp"
#include "vfs/gob_virtual_container.hpp"
#include "io/std_output_stream.hpp"
#include <iostream>
#include <iomanip>

namespace gorc {

    class gob_program : public program {
    private:
        // Archive modes
        std::string archive_file;

        // Sub-commands
        bool do_list = false;
        std::string extract_file;

    public:
        virtual void create_options(options &opts) override
        {
            // Archive modes
            opts.insert(make_value_option("file", archive_file));
            opts.emplace_constraint<required_option>("file");

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
            if(!archive_file.empty()) {
                // Handle single archive
                return single_archive_main();
            }

            // LCOV_EXCL_START
            // Not possible - guards against future changes
            LOG_FATAL("unhandled option");
            // LCOV_EXCL_STOP
        }

        int single_archive_main()
        {
            gob_virtual_container gob(archive_file);

            if(do_list) {
                return single_archive_list(gob);
            }
            else if(!extract_file.empty()) {
                return single_archive_extract_file(gob);
            }

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
