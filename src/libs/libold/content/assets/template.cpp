#include "log/log.hpp"
#include "template.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "libold/base/content/content_manager.hpp"
#include <unordered_map>
#include <functional>
#include <type_traits>

namespace gorc {
namespace content {
namespace assets {

class template_parser_args {
public:
    text::tokenizer& tok;
    content::content_manager& content;
    service_registry const &services;
    const std::unordered_map<std::string, int>& templates;

    template_parser_args(text::tokenizer& tok, content::content_manager& content,
            service_registry const &services,
            const std::unordered_map<std::string, int>& templates) :
                tok(tok), content(content), services(services),
                templates(templates) {
        return;
    }
};

#define TPP_ARGS thing_template& tpl, template_parser_args& args

static const std::unordered_map<std::string, flags::thing_type> TemplateTypeMap {
    { "actor", flags::thing_type::Actor },
    { "weapon", flags::thing_type::Weapon },
    { "debris", flags::thing_type::Debris },
    { "item", flags::thing_type::Item },
    { "explosion", flags::thing_type::Explosion },
    { "cog", flags::thing_type::cog },
    { "ghost", flags::thing_type::ghost },
    { "corpse", flags::thing_type::Corpse },
    { "player", flags::thing_type::Player },
    { "particle", flags::thing_type::Particle }
};

static const std::unordered_map<std::string, flags::move_type> MoveTypeMap {
    { "none", flags::move_type::none },
    { "physics", flags::move_type::physics },
    { "path", flags::move_type::Path }
};

template <typename T> void tpl_value_mapper(const std::unordered_map<std::string, T>& map, T& value,
        const T& defaultvalue, text::tokenizer& tok) {
    std::string key = tok.get_space_delimited_string();
    std::transform(key.begin(), key.end(), key.begin(), tolower);
    auto it = map.find(key);
    if(it == map.end()) {
        diagnostic_context dc(nullptr,
                              tok.get_internal_token_location().first_line,
                              tok.get_internal_token_location().first_col);
        LOG_WARNING(format("unknown parameter value '%s'") % key);
        value = defaultvalue;
    }
    else {
        value = it->second;
    }
}

template <typename T> void tpl_number_mapper(T& value, const T& defaultvalue, text::tokenizer& tok) {
    text::token t;
    tok.get_token(t);

    if(t.type == text::token_type::floating || t.type == text::token_type::integer || t.type == text::token_type::hex_integer) {
        value = t.get_numeric_value<T>();
    }
    else {
        diagnostic_context dc(nullptr,
                              tok.get_internal_token_location().first_line,
                              tok.get_internal_token_location().first_col);
        LOG_WARNING("expected numeric value");
        value = defaultvalue;
    }
}

template <typename T> void tpl_enum_mapper(T& value, const T& defaultvalue, text::tokenizer& tok) {
    text::token t;
    tok.get_token(t);

    if(t.type == text::token_type::floating || t.type == text::token_type::integer || t.type == text::token_type::hex_integer) {
        value = static_cast<T>(t.get_numeric_value<typename std::underlying_type<T>::type>());
    }
    else {
        diagnostic_context dc(nullptr,
                              tok.get_internal_token_location().first_line,
                              tok.get_internal_token_location().first_col);
        LOG_WARNING("expected numeric value");
        value = defaultvalue;
    }
}

template <typename T> void tpl_flag_mapper(flag_set<T>& value, const flag_set<T>& defaultvalue, text::tokenizer& tok) {
    text::token t;
    tok.get_token(t);

    if(t.type == text::token_type::floating || t.type == text::token_type::integer || t.type == text::token_type::hex_integer) {
        value = flag_set<T>(t.get_numeric_value<typename std::underlying_type<T>::type>());
    }
    else {
        diagnostic_context dc(nullptr,
                              tok.get_internal_token_location().first_line,
                              tok.get_internal_token_location().first_col);
        LOG_WARNING("expected numeric value");
        value = defaultvalue;
    }
}

void tpl_vector_mapper(vector<3>& value, text::tokenizer& tok) {
    tok.assert_punctuator("(");
    get<0>(value) = tok.get_number<float>();
    tok.assert_punctuator("/");
    get<1>(value) = tok.get_number<float>();
    tok.assert_punctuator("/");
    get<2>(value) = tok.get_number<float>();
    tok.assert_punctuator(")");
}

void tpl_quaternion_mapper(quaternion<float>& value, text::tokenizer& tok) {
    vector<3> v;
    tpl_vector_mapper(v, tok);

    // Convert orientation vector into quaternion:
    value = make_euler(v);
}

void tpl_template_mapper(int& value, int, const std::unordered_map<std::string, int>& templates,
        text::tokenizer& tok) {
    std::string tpl_name = tok.get_space_delimited_string();
    std::transform(tpl_name.begin(), tpl_name.end(), tpl_name.begin(), tolower);

    auto tpl_it = templates.find(tpl_name);
    if(tpl_it == templates.end()) {
        diagnostic_context dc(nullptr,
                              tok.get_internal_token_location().first_line,
                              tok.get_internal_token_location().first_col);
        LOG_WARNING("expected template name");
        value = 0;
    }
    else {
        value = tpl_it->second;
    }
}

template <typename T> void tpl_asset_loader(maybe<asset_ref<T>>& value, text::tokenizer& tok, content::content_manager& manager) {
    std::string fn = tok.get_space_delimited_string();
    if(boost::iequals(fn, "none")) {
        value = nothing;
    }
    else {
        try {
            value = manager.load<T>(fn);
        }
        catch(...) {
            value = nothing;
        }
    }
}

void tpl_add_frame(thing_template& tpl, text::tokenizer& tok) {
    tok.assert_punctuator("(");
    float x = tok.get_number<float>();
    tok.assert_punctuator("/");
    float y = tok.get_number<float>();
    tok.assert_punctuator("/");
    float z = tok.get_number<float>();

    text::token t;
    tok.get_token(t);

    if(t.value == ":") {
        float p = tok.get_number<float>();
        tok.assert_punctuator("/");
        float a = tok.get_number<float>();
        tok.assert_punctuator("/");
        float r = tok.get_number<float>();
        tok.assert_punctuator(")");

        tpl.frames.emplace_back(make_vector(x, y, z), make_vector(p, a, r));
    }
    else {
        tpl.frames.emplace_back(make_vector(x, y, z), make_zero_vector<3, float>());
    }
}

using TemplateParameterParser = std::function<void(thing_template&, template_parser_args&)>;

static const std::unordered_map<std::string, TemplateParameterParser> TemplateParameterParserMap {
    { "actorflags", [](TPP_ARGS) { tpl_flag_mapper(tpl.actor_flags, flag_set<flags::actor_flag>(), args.tok); }},
    { "angvel", [](TPP_ARGS) { tpl_vector_mapper(tpl.ang_vel, args.tok); }},
    { "cog", [](TPP_ARGS) { tpl_asset_loader(tpl.cog, args.tok, args.content); }},
    { "collide", [](TPP_ARGS) { tpl_enum_mapper(tpl.collide, flags::collide_type::none, args.tok); }},
    { "creatething", [](TPP_ARGS) { tpl_template_mapper(tpl.create_thing, 0, args.templates, args.tok); }},
    { "damage", [](TPP_ARGS) { tpl_number_mapper(tpl.damage, 0.0f, args.tok); }},
    { "damageclass", [](TPP_ARGS) { tpl_flag_mapper(tpl.damage_class, flag_set<flags::damage_flag>(), args.tok); }},
    { "explode", [](TPP_ARGS) { tpl_template_mapper(tpl.explode, 0, args.templates, args.tok); }},
    { "eyeoffset", [](TPP_ARGS) { tpl_vector_mapper(tpl.eye_offset, args.tok); }},
    { "fleshhit", [](TPP_ARGS) { tpl_template_mapper(tpl.flesh_hit, 0, args.templates, args.tok); }},
    { "frame", [](TPP_ARGS) { tpl_add_frame(tpl, args.tok); }},
    { "headpitch", [](TPP_ARGS) { tpl_number_mapper(tpl.head_pitch, 0.0f, args.tok); }},
    { "health", [](TPP_ARGS) { tpl_number_mapper(tpl.health, 100.0f, args.tok); }},
    { "height", [](TPP_ARGS) { tpl_number_mapper(tpl.height, 0.18f, args.tok); }},
    { "jumpspeed", [](TPP_ARGS) { tpl_number_mapper(tpl.jump_speed, 1.5f, args.tok); }},
    { "light", [](TPP_ARGS) { tpl_number_mapper(tpl.light, 0.0f, args.tok); }},
    { "lightintensity", [](TPP_ARGS) { tpl_number_mapper(tpl.light_intensity, 0.0f, args.tok); }},
    { "lightoffset", [](TPP_ARGS) { tpl_vector_mapper(tpl.light_offset, args.tok); }},
    { "mass", [](TPP_ARGS) { tpl_number_mapper(tpl.mass, 2.0f, args.tok); }},
    { "maxheadpitch", [](TPP_ARGS) { tpl_number_mapper(tpl.max_head_pitch, 80.0f, args.tok); }},
    { "maxhealth", [](TPP_ARGS) { tpl_number_mapper(tpl.max_health, 100.0f, args.tok); }},
    { "maxlight", [](TPP_ARGS) { tpl_number_mapper(tpl.light, 1.0f, args.tok); }},
    { "maxrotthrust", [](TPP_ARGS) { tpl_number_mapper(tpl.max_rot_thrust, 180.0f, args.tok); }},
    { "maxrotvel", [](TPP_ARGS) { tpl_number_mapper(tpl.max_rot_vel, 200.0f, args.tok); }},
    { "maxthrust", [](TPP_ARGS) { tpl_number_mapper(tpl.max_thrust, 2.00f, args.tok); }},
    { "maxvel", [](TPP_ARGS) { tpl_number_mapper(tpl.max_vel, 1.0f, args.tok); }},
    { "mindamage", [](TPP_ARGS) { tpl_number_mapper(tpl.min_damage, 0.0f, args.tok); }},
    { "minheadpitch", [](TPP_ARGS) { tpl_number_mapper(tpl.min_head_pitch, -80.0f, args.tok); }},
    { "model3d", [](TPP_ARGS) { tpl_asset_loader(tpl.model_3d, args.tok, args.content); }},
    { "move", [](TPP_ARGS) { tpl_value_mapper(MoveTypeMap, tpl.move, flags::move_type::none, args.tok); }},
    { "movesize", [](TPP_ARGS) { tpl_number_mapper(tpl.move_size, 0.05f, args.tok); }},
    { "numframes", [](thing_template&, template_parser_args& args) { /* Silently consume numframes */ args.tok.get_number<int>(); }},
    { "orient", [](TPP_ARGS) { tpl_quaternion_mapper(tpl.orient, args.tok); }},
    { "physflags", [](TPP_ARGS) { tpl_flag_mapper(tpl.physics_flags, flag_set<flags::physics_flag>(), args.tok); }},
    { "puppet", [](TPP_ARGS) { tpl_asset_loader(tpl.pup, args.tok, args.content); }},
    { "rotthrust", [](TPP_ARGS) { tpl_vector_mapper(tpl.rot_thrust, args.tok); }},
    { "size", [](TPP_ARGS) { tpl_number_mapper(tpl.size, 0.05f, args.tok); }},
    { "soundclass", [](TPP_ARGS) { tpl_asset_loader(tpl.sound_class, args.tok, args.content); }},
    { "sprite", [](TPP_ARGS) { tpl_asset_loader(tpl.spr, args.tok, args.content); }},
    { "thingflags", [](TPP_ARGS) { tpl_flag_mapper(tpl.flags, flag_set<flags::thing_flag>(), args.tok); }},
    { "thrust", [](TPP_ARGS) { tpl_vector_mapper(tpl.thrust, args.tok); }},
    { "timer", [](TPP_ARGS) { tpl_number_mapper(tpl.timer, 0.0f, args.tok); }},
    { "type", [](TPP_ARGS) { tpl_value_mapper(TemplateTypeMap, tpl.type, flags::thing_type::Free, args.tok); }},
    { "typeflags", [](TPP_ARGS) { tpl_number_mapper(tpl.type_flags, 0, args.tok); }},
    { "vel", [](TPP_ARGS) { tpl_vector_mapper(tpl.vel, args.tok); }}
};

#undef TPP_ARGS

}
}
}

void gorc::content::assets::thing_template::parse_args(text::tokenizer& tok, content::content_manager& manager, service_registry const &services,
        const std::unordered_map<std::string, int>& templates) {
    template_parser_args tp_args(tok, manager, services, templates);

    bool oldReportEOL = tok.get_report_eol();
    tok.set_report_eol(true);

    text::token t;
    while(true) {
        tok.get_token(t);

        if(t.type == text::token_type::end_of_file || t.type == text::token_type::end_of_line) {
            break;
        }
        else {
            tok.assert_punctuator("=");

            std::transform(t.value.begin(), t.value.end(), t.value.begin(), tolower);
            auto it = TemplateParameterParserMap.find(t.value);
            if(it == TemplateParameterParserMap.end()) {
                diagnostic_context dc(nullptr, t.location.first_line, t.location.first_col);
                LOG_WARNING(format("unrecognized template param '%s'") % t.value);
                tok.get_space_delimited_string();
            }
            else {
                it->second(*this, tp_args);
            }
        }
    }

    tok.set_report_eol(oldReportEOL);
}
