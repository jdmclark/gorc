#include "template.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "framework/content/manager.h"
#include <unordered_map>
#include <functional>
#include <type_traits>

namespace gorc {
namespace content {
namespace assets {

class template_parser_args {
public:
    text::tokenizer& tok;
    content::manager& content;
    const assets::colormap& colormap;
    const cog::compiler& compiler;
    const std::unordered_map<std::string, int>& templates;
    diagnostics::report& report;

    template_parser_args(text::tokenizer& tok, content::manager& content,
            const assets::colormap& colormap, const cog::compiler& compiler,
            const std::unordered_map<std::string, int>& templates,
            diagnostics::report& report) :
                tok(tok), content(content), colormap(colormap), compiler(compiler),
                templates(templates), report(report) {
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
        const T& defaultvalue, text::tokenizer& tok, diagnostics::report& report) {
    std::string key = tok.get_space_delimited_string();
    std::transform(key.begin(), key.end(), key.begin(), tolower);
    auto it = map.find(key);
    if(it == map.end()) {
        report.add_warning("thing_template::ParseArgs::TemplateParametervalueMapper", boost::str(boost::format("unknown parameter value \'%s\'") % key),
                tok.get_internal_token_location());
        value = defaultvalue;
    }
    else {
        value = it->second;
    }
}

template <typename T> void tpl_number_mapper(T& value, const T& defaultvalue, text::tokenizer& tok, diagnostics::report& report) {
    text::token t;
    tok.get_token(t);

    if(t.type == text::token_type::floating || t.type == text::token_type::integer || t.type == text::token_type::hex_integer) {
        value = t.get_numeric_value<T>();
    }
    else {
        report.add_warning("thing_template::ParseArgs::TemplateParameterNumberMapper", "expected numeric value", t.location);
        value = defaultvalue;
    }
}

template <typename T> void tpl_enum_mapper(T& value, const T& defaultvalue, text::tokenizer& tok, diagnostics::report& report) {
    text::token t;
    tok.get_token(t);

    if(t.type == text::token_type::floating || t.type == text::token_type::integer || t.type == text::token_type::hex_integer) {
        value = static_cast<T>(t.get_numeric_value<typename std::underlying_type<T>::type>());
    }
    else {
        report.add_warning("thing_template::ParseArgs::TemplateParameterEnumMapper", "expected numeric value", t.location);
        value = defaultvalue;
    }
}

template <typename T> void tpl_flag_mapper(flag_set<T>& value, const flag_set<T>& defaultvalue, text::tokenizer& tok, diagnostics::report& report) {
    text::token t;
    tok.get_token(t);

    if(t.type == text::token_type::floating || t.type == text::token_type::integer || t.type == text::token_type::hex_integer) {
        value = flag_set<T>(t.get_numeric_value<typename std::underlying_type<T>::type>());
    }
    else {
        report.add_warning("thing_template::ParseArgs::TemplateParameterFlagMapper", "expected numeric value", t.location);
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
        text::tokenizer& tok, diagnostics::report& report) {
    std::string tpl_name = tok.get_space_delimited_string();
    std::transform(tpl_name.begin(), tpl_name.end(), tpl_name.begin(), tolower);

    auto tpl_it = templates.find(tpl_name);
    if(tpl_it == templates.end()) {
        report.add_warning("thing_template::ParseArgs::TemplateParameterTemplateMapper", "expected template name", tok.get_internal_token_location());
        value = 0;
    }
    else {
        value = tpl_it->second;
    }
}

template <typename T, typename... U> void tpl_asset_loader(T const*& value, text::tokenizer& tok, content::manager& manager, U... args) {
    std::string fn = tok.get_space_delimited_string();
    if(boost::iequals(fn, "none")) {
        value = nullptr;
    }
    else {
        try {
            value = &manager.load<T>(fn, args...);
        }
        catch(...) {
            value = nullptr;
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
    { "actorflags", [](TPP_ARGS) { tpl_flag_mapper(tpl.actor_flags, flag_set<flags::actor_flag>(), args.tok, args.report); }},
    { "angvel", [](TPP_ARGS) { tpl_vector_mapper(tpl.ang_vel, args.tok); }},
    { "cog", [](TPP_ARGS) { tpl_asset_loader(tpl.cog, args.tok, args.content, args.compiler); }},
    { "collide", [](TPP_ARGS) { tpl_enum_mapper(tpl.collide, flags::collide_type::none, args.tok, args.report); }},
    { "creatething", [](TPP_ARGS) { tpl_template_mapper(tpl.create_thing, 0, args.templates, args.tok, args.report); }},
    { "damage", [](TPP_ARGS) { tpl_number_mapper(tpl.damage, 0.0f, args.tok, args.report); }},
    { "damageclass", [](TPP_ARGS) { tpl_flag_mapper(tpl.damage_class, flag_set<flags::damage_flag>(), args.tok, args.report); }},
    { "explode", [](TPP_ARGS) { tpl_template_mapper(tpl.explode, 0, args.templates, args.tok, args.report); }},
    { "eyeoffset", [](TPP_ARGS) { tpl_vector_mapper(tpl.eye_offset, args.tok); }},
    { "fleshhit", [](TPP_ARGS) { tpl_template_mapper(tpl.flesh_hit, 0, args.templates, args.tok, args.report); }},
    { "frame", [](TPP_ARGS) { tpl_add_frame(tpl, args.tok); }},
    { "headpitch", [](TPP_ARGS) { tpl_number_mapper(tpl.head_pitch, 0.0f, args.tok, args.report); }},
    { "health", [](TPP_ARGS) { tpl_number_mapper(tpl.health, 100.0f, args.tok, args.report); }},
    { "height", [](TPP_ARGS) { tpl_number_mapper(tpl.height, 0.18f, args.tok, args.report); }},
    { "jumpspeed", [](TPP_ARGS) { tpl_number_mapper(tpl.jump_speed, 1.5f, args.tok, args.report); }},
    { "light", [](TPP_ARGS) { tpl_number_mapper(tpl.light, 0.0f, args.tok, args.report); }},
    { "lightintensity", [](TPP_ARGS) { tpl_number_mapper(tpl.light_intensity, 0.0f, args.tok, args.report); }},
    { "lightoffset", [](TPP_ARGS) { tpl_vector_mapper(tpl.light_offset, args.tok); }},
    { "mass", [](TPP_ARGS) { tpl_number_mapper(tpl.mass, 2.0f, args.tok, args.report); }},
    { "maxheadpitch", [](TPP_ARGS) { tpl_number_mapper(tpl.max_head_pitch, 80.0f, args.tok, args.report); }},
    { "maxhealth", [](TPP_ARGS) { tpl_number_mapper(tpl.max_health, 100.0f, args.tok, args.report); }},
    { "maxlight", [](TPP_ARGS) { tpl_number_mapper(tpl.light, 1.0f, args.tok, args.report); }},
    { "maxrotthrust", [](TPP_ARGS) { tpl_number_mapper(tpl.max_rot_thrust, 180.0f, args.tok, args.report); }},
    { "maxrotvel", [](TPP_ARGS) { tpl_number_mapper(tpl.max_rot_vel, 200.0f, args.tok, args.report); }},
    { "maxthrust", [](TPP_ARGS) { tpl_number_mapper(tpl.max_thrust, 2.00f, args.tok, args.report); }},
    { "maxvel", [](TPP_ARGS) { tpl_number_mapper(tpl.max_vel, 1.0f, args.tok, args.report); }},
    { "mindamage", [](TPP_ARGS) { tpl_number_mapper(tpl.min_damage, 0.0f, args.tok, args.report); }},
    { "minheadpitch", [](TPP_ARGS) { tpl_number_mapper(tpl.min_head_pitch, -80.0f, args.tok, args.report); }},
    { "model3d", [](TPP_ARGS) { tpl_asset_loader(tpl.model_3d, args.tok, args.content, args.colormap); }},
    { "move", [](TPP_ARGS) { tpl_value_mapper(MoveTypeMap, tpl.move, flags::move_type::none, args.tok, args.report); }},
    { "movesize", [](TPP_ARGS) { tpl_number_mapper(tpl.move_size, 0.05f, args.tok, args.report); }},
    { "numframes", [](thing_template&, template_parser_args& args) { /* Silently consume numframes */ args.tok.get_number<int>(); }},
    { "orient", [](TPP_ARGS) { tpl_quaternion_mapper(tpl.orient, args.tok); }},
    { "physflags", [](TPP_ARGS) { tpl_flag_mapper(tpl.physics_flags, flag_set<flags::physics_flag>(), args.tok, args.report); }},
    { "puppet", [](TPP_ARGS) { tpl_asset_loader(tpl.pup, args.tok, args.content); }},
    { "rotthrust", [](TPP_ARGS) { tpl_vector_mapper(tpl.rot_thrust, args.tok); }},
    { "size", [](TPP_ARGS) { tpl_number_mapper(tpl.size, 0.05f, args.tok, args.report); }},
    { "soundclass", [](TPP_ARGS) { tpl_asset_loader(tpl.sound_class, args.tok, args.content); }},
    { "sprite", [](TPP_ARGS) { tpl_asset_loader(tpl.spr, args.tok, args.content, args.colormap); }},
    { "thingflags", [](TPP_ARGS) { tpl_flag_mapper(tpl.flags, flag_set<flags::thing_flag>(), args.tok, args.report); }},
    { "thrust", [](TPP_ARGS) { tpl_vector_mapper(tpl.thrust, args.tok); }},
    { "timer", [](TPP_ARGS) { tpl_number_mapper(tpl.timer, 0.0f, args.tok, args.report); }},
    { "type", [](TPP_ARGS) { tpl_value_mapper(TemplateTypeMap, tpl.type, flags::thing_type::Free, args.tok, args.report); }},
    { "typeflags", [](TPP_ARGS) { tpl_number_mapper(tpl.type_flags, 0, args.tok, args.report); }},
    { "vel", [](TPP_ARGS) { tpl_vector_mapper(tpl.vel, args.tok); }}
};

#undef TPP_ARGS

}
}
}

void gorc::content::assets::thing_template::parse_args(text::tokenizer& tok, content::manager& manager, const colormap& cmp,
        const cog::compiler& compiler, const std::unordered_map<std::string, int>& templates, diagnostics::report& report) {
    template_parser_args tp_args(tok, manager, cmp, compiler, templates, report);

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
                report.add_warning("thing_template::ParseArgs", boost::str(boost::format("unrecognized template param \'%s\'") % t.value), t.location);
                tok.get_space_delimited_string();
            }
            else {
                it->second(*this, tp_args);
            }
        }
    }

    tok.set_report_eol(oldReportEOL);
}
