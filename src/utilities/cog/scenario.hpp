#pragma once

#include "cog/script/value.hpp"
#include "text/json_input_stream.hpp"
#include "utility/time.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

namespace gorc {

    class cog_scenario_instance {
    public:
        std::string cog_filename;
        std::vector<cog::value> init;

        cog_scenario_instance(deserialization_constructor_tag, json_input_stream &);
    };

    class cog_scenario {
    public:
        std::vector<cog_scenario_instance> cog_files;
        std::unordered_map<std::string, cog::value> resources;
        time_delta time_step = 1.0s;
        time_delta max_time = 0.0s;

        cog_scenario(deserialization_constructor_tag, json_input_stream &);
    };

}
