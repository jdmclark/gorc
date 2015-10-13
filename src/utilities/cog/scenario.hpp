#pragma once

#include "cog/script/value.hpp"
#include "text/json_input_stream.hpp"
#include <unordered_map>
#include <string>
#include <vector>

namespace gorc {

    class cog_scenario_instance {
    public:
        std::string cog_filename;
        std::vector<cog::value> init;

        cog_scenario_instance(json_deserialization_constructor, json_input_stream &);
    };

    class cog_scenario {
    public:
        std::vector<cog_scenario_instance> cog_files;
        std::unordered_map<std::string, cog::value> resources;

        cog_scenario(json_deserialization_constructor, json_input_stream &);
    };

}
