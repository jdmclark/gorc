#include "io/binary_input_stream.hpp"
#include "io/native_file.hpp"
#include "io/path.hpp"
#include "io/std_input_stream.hpp"
#include "jk/content/colormap.hpp"
#include "jk/content/raw_material.hpp"
#include "log/log.hpp"
#include "program/program.hpp"
#include "text/extract_path.hpp"
#include "text/json_output_stream.hpp"
#include <boost/filesystem.hpp>

namespace gorc {

    class mat_program : public program {
    public:
        path extract_path;
        path colormap_path;

        virtual void create_options(options &opts) override
        {
            opts.insert(make_value_option("extract-to", extract_path));
            opts.insert(make_value_option("colormap", colormap_path));
            return;
        }

        virtual int run() override
        {
            diagnostic_context dc("<stdin>");

            std_input_stream is;
            is.reopen_as_binary();

            binary_input_stream bis(is);
            raw_material mat(deserialization_constructor, bis);

            if(extract_path.empty()) {
                return EXIT_SUCCESS;
            }

            service_registry services;

            std::unique_ptr<colormap> cmp;

            if(!colormap_path.empty()) {
                auto cmp_is = make_native_read_only_file(colormap_path);
                binary_input_stream bis(*cmp_is);
                cmp = std::make_unique<colormap>(deserialization_constructor, bis);
                services.add<colormap>(*cmp);
            }

            // Materials contain multiple data chunks.
            // JSON serialization requires supplementary output path.
            extract_path.replace_extension();
            gorc::extract_path extract_path_service(extract_path);
            services.add<gorc::extract_path>(extract_path_service);

            boost::filesystem::create_directories(extract_path.parent_path());

            path metadata_path = extract_path;
            metadata_path.replace_extension(".json");

            auto os = make_native_file(metadata_path);
            json_output_stream jos(*os, services);
            json_serialize(jos, mat);

            return EXIT_SUCCESS;
        }
    };
}

MAKE_MAIN(gorc::mat_program)
