#include "scenario.hpp"
#include "text/json_specification.hpp"
#include "log/log.hpp"
#include "read_cog_value.hpp"
#include <list>

using namespace gorc;

namespace {

    json_specification<cog_scenario_instance> cog_scenario_instance_spec(
            /* Members */
            {
                { "file", make_json_member(&cog_scenario_instance::cog_filename) },

                { "init", [](json_input_stream &f, cog_scenario_instance &inst) {
                        json_deserialize_array(f, [&](json_input_stream &f) {
                            inst.init.push_back(read_cog_value(f));
                        });
                    }
                },
            },

            /* Required members */
            { "file" }
        );

    std::unique_ptr<cog_scenario_event> deserialize_event(json_input_stream &f)
    {
        return gorc::json_deserialize_heterogeneous(f, [&](json_input_stream &f,
                                                           std::string const &type)
                    -> std::unique_ptr<cog_scenario_event> {
                if(type == "time") {
                    return std::make_unique<time_step_event>(deserialization_constructor, f);
                }
                else if(type == "send_linked") {
                    return std::make_unique<send_linked_event>(deserialization_constructor, f);
                }
                else {
                    LOG_FATAL(format("unknown scenario event type '%s'") % type);
                }
            });
    }

    json_specification<cog_scenario> cog_scenario_spec(
            /* Members */
            {
                { "instances", [](json_input_stream &f, cog_scenario &scn) {
                        json_deserialize_array(f, [&](json_input_stream &f) {
                            scn.cog_files.emplace_back(deserialization_constructor, f);
                        });
                    }
                },
                { "resources", [](json_input_stream &f, cog_scenario &scn) {
                        json_deserialize_members(f, [&](json_input_stream &f,
                                                        std::string const &name) {
                                scn.resources.emplace(name, read_cog_value(f));
                            });
                    }
                },
                { "events", [](json_input_stream &f, cog_scenario &scn) {
                        json_deserialize_array(f, [&](json_input_stream &f) {
                            scn.events.push_back(deserialize_event(f));
                        });
                    }
                }
            }
        );

}

cog_scenario_instance::cog_scenario_instance(deserialization_constructor_tag,
                                             json_input_stream &f)
{
    json_deserialize_with_specification(f, cog_scenario_instance_spec, *this);
}

cog_scenario::cog_scenario(deserialization_constructor_tag,
                           json_input_stream &f)
{
    json_deserialize_with_specification(f, cog_scenario_spec, *this);
}
