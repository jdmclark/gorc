#include "program/program.hpp"
#include "jkcontent/colormap.hpp"
#include "io/binary_input_stream.hpp"
#include "text/json_output_stream.hpp"
#include "text/extract_path.hpp"
#include "io/std_input_stream.hpp"
#include "log/log.hpp"
#include "io/path.hpp"
#include "io/native_file.hpp"
#include <boost/filesystem.hpp>

namespace gorc {

    class cmp_program : public program {
    public:
        path extract_path;

        virtual void create_options(options &opts) override
        {
            opts.insert(make_value_option("extract-to", extract_path));

            opts.emplace_constraint<required_option>("extract-to");
            return;
        }

        virtual int run() override
        {
            diagnostic_context dc("<stdin>");

            std_input_stream is;
            is.reopen_as_binary();

            binary_input_stream bis(is);
            colormap cmp(deserialization_constructor, bis);

            service_registry services;

            // Colormaps contain multiple data chunks.
            // JSON serialization requires supplementary output path.
            extract_path.replace_extension();
            gorc::extract_path extract_path_service(extract_path);
            services.add<gorc::extract_path>(extract_path_service);

            boost::filesystem::create_directories(extract_path.parent_path());

            path metadata_path = extract_path;
            metadata_path.replace_extension(".json");

            auto os = make_native_file(metadata_path);
            json_output_stream jos(*os, services);
            json_serialize(jos, cmp);

            return EXIT_SUCCESS;
        }
    };

}

MAKE_MAIN(gorc::cmp_program)
