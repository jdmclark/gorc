#include "scenario.hpp"
#include "text/json_specification.hpp"
#include "log/log.hpp"
#include <list>

using namespace gorc;

namespace {

    std::list<std::string> string_constant_cache;

    cog::value read_cog_value(json_input_stream &f)
    {
        return json_deserialize_heterogeneous(f, [](json_input_stream &f,
                                                    std::string const &type) -> cog::value {
                    if(type == "void") {
                        return cog::value();
                    }
                    else if(type == "int") {
                        return cog::value(json_deserialize<int>(f));
                    }
                    else if(type == "float") {
                        return cog::value(json_deserialize<float>(f));
                    }
                    else if(type == "vector") {
                        vector<3> v;
                        int idx = 0;
                        json_deserialize_array(f, [&](json_input_stream &f) {
                            switch(idx) {
                            case 0:
                                get<0>(v) = json_deserialize<float>(f);
                                break;

                            case 1:
                                get<1>(v) = json_deserialize<float>(f);
                                break;

                            case 2:
                                get<2>(v) = json_deserialize<float>(f);
                                break;

                            default:
                                LOG_FATAL("too many elements in vector");
                            }

                            ++idx;
                        });

                        if(idx != 3) {
                            LOG_FATAL("not enough elements in vector");
                        }

                        return cog::value(v);
                    }
                    else if(type == "sector") {
                        return cog::value(cog::sector_id(json_deserialize<int>(f)));
                    }
                    else if(type == "surface") {
                        return cog::value(cog::surface_id(json_deserialize<int>(f)));
                    }
                    else if(type == "thing") {
                        return cog::value(cog::thing_id(json_deserialize<int>(f)));
                    }
                    else if(type == "ai") {
                        return cog::value(cog::ai_id(json_deserialize<int>(f)));
                    }
                    else if(type == "cog") {
                        return cog::value(cog::cog_id(json_deserialize<int>(f)));
                    }
                    else if(type == "keyframe") {
                        return cog::value(cog::keyframe_id(json_deserialize<int>(f)));
                    }
                    else if(type == "material") {
                        return cog::value(cog::material_id(json_deserialize<int>(f)));
                    }
                    else if(type == "model") {
                        return cog::value(cog::model_id(json_deserialize<int>(f)));
                    }
                    else if(type == "sound") {
                        return cog::value(cog::sound_id(json_deserialize<int>(f)));
                    }
                    else if(type == "template") {
                        return cog::value(cog::thing_template_id(json_deserialize<int>(f)));
                    }
                    else if(type == "string") {
                        string_constant_cache.push_back(json_deserialize<std::string>(f));
                        return cog::value(string_constant_cache.back().c_str());
                    }

                    LOG_FATAL(format("unhandled type %s") % type);
                });
    }

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

    json_specification<cog_scenario> cog_scenario_spec(
            /* Members */
            {
                { "instances", [](json_input_stream &f, cog_scenario &scn) {
                        json_deserialize_array(f, [&](json_input_stream &f) {
                            scn.cog_files.emplace_back(json_deserialization_constructor_tag, f);
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
                { "timestep", make_json_member(&cog_scenario::time_step) },
                { "timemax", make_json_member(&cog_scenario::max_time) }
            }
        );

}

cog_scenario_instance::cog_scenario_instance(json_deserialization_constructor,
                                             json_input_stream &f)
{
    json_deserialize_with_specification(f, cog_scenario_instance_spec, *this);
}

cog_scenario::cog_scenario(json_deserialization_constructor,
                           json_input_stream &f)
{
    json_deserialize_with_specification(f, cog_scenario_spec, *this);
}
