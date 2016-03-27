#include "read_cog_value.hpp"
#include <list>
#include <string>

namespace {

    std::list<std::string> string_constant_cache;

}

gorc::cog::value gorc::read_cog_value(json_input_stream &f)
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
            return cog::value(sector_id(json_deserialize<int>(f)));
        }
        else if(type == "surface") {
            return cog::value(surface_id(json_deserialize<int>(f)));
        }
        else if(type == "thing") {
            return cog::value(thing_id(json_deserialize<int>(f)));
        }
        else if(type == "ai") {
            return cog::value(ai_id(json_deserialize<int>(f)));
        }
        else if(type == "cog") {
            return cog::value(cog_id(json_deserialize<int>(f)));
        }
        else if(type == "keyframe") {
            return cog::value(keyframe_id(json_deserialize<int>(f)));
        }
        else if(type == "material") {
            return cog::value(material_id(json_deserialize<int>(f)));
        }
        else if(type == "model") {
            return cog::value(model_id(json_deserialize<int>(f)));
        }
        else if(type == "sound") {
            return cog::value(sound_id(json_deserialize<int>(f)));
        }
        else if(type == "template") {
            return cog::value(thing_template_id(json_deserialize<int>(f)));
        }
        else if(type == "string") {
            string_constant_cache.push_back(json_deserialize<std::string>(f));
            return cog::value(string_constant_cache.back().c_str());
        }

        LOG_FATAL(format("unhandled type %s") % type);
    });
}
