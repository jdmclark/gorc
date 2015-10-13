#include "type.hpp"
#include "utility/enum_hash.hpp"
#include <stdexcept>
#include <unordered_map>

using namespace gorc;
using namespace gorc::cog;

namespace {
    std::unordered_map<value_type, char const *, enum_hash<value_type>> value_type_string_map {
        { value_type::nothing, "void" },
        { value_type::dynamic, "var" },
        { value_type::string, "string" },
        { value_type::message, "message" },
        { value_type::boolean, "bool" },
        { value_type::integer, "int" },
        { value_type::floating, "float" },
        { value_type::vector, "vector" },
        { value_type::sector, "sector" },
        { value_type::surface, "surface" },
        { value_type::thing, "thing" },
        { value_type::player, "player" },
        { value_type::ai, "ai" },
        { value_type::cog, "cog" },
        { value_type::colormap, "colormap" },
        { value_type::keyframe, "keyframe" },
        { value_type::material, "material" },
        { value_type::model, "model" },
        { value_type::sound, "sound" },
        { value_type::thing_template, "template" }
    };
}

char const* gorc::cog::as_string(value_type vt)
{
    auto it = value_type_string_map.find(vt);
    if(it != value_type_string_map.end()) {
        return it->second;
    }

    throw std::range_error("not a valid value_type");
}

bool gorc::cog::is_id_type(value_type vt)
{
    switch(vt) {
    case value_type::dynamic:
    case value_type::sector:
    case value_type::surface:
    case value_type::thing:
    case value_type::player:
    case value_type::ai:
    case value_type::cog:
    case value_type::colormap:
    case value_type::keyframe:
    case value_type::material:
    case value_type::model:
    case value_type::sound:
    case value_type::thing_template:
    case value_type::message:
        return true;

    default:
        return false;
    }
}

bool gorc::cog::is_resource_id_type(value_type vt)
{
    switch(vt) {
    case value_type::ai:
    case value_type::cog:
    case value_type::colormap:
    case value_type::keyframe:
    case value_type::material:
    case value_type::model:
    case value_type::sound:
    case value_type::thing_template:
        return true;

    default:
        return false;
    }
}

bool gorc::cog::is_scalar_type(value_type vt)
{
    switch(vt) {
    case value_type::boolean:
    case value_type::integer:
    case value_type::floating:
    case value_type::dynamic:
        return true;

    default:
        return false;
    }
}

bool gorc::cog::is_truth_value_type(value_type vt)
{
    switch(vt) {
    case value_type::boolean:
    case value_type::integer:
    case value_type::floating:
    case value_type::dynamic:
        return true;

    default:
        return false;
    }
}

bool gorc::cog::can_convert_type(value_type vt_initial, value_type vt_target)
{
    return (vt_initial == value_type::dynamic) ||
           (vt_target == value_type::dynamic) ||
           (vt_initial == vt_target) ||
           (is_scalar_type(vt_initial) && is_scalar_type(vt_target)) ||
           (is_id_type(vt_initial) && is_scalar_type(vt_target));
}

gorc::cog::value_type gorc::cog::get_closest_numeric_type(value_type type)
{
    if(type == value_type::floating) {
        return value_type::floating;
    }
    else {
        return value_type::integer;
    }
}

gorc::cog::value_type gorc::cog::get_common_numeric_type(value_type left, value_type right)
{
    if(left == value_type::floating || right == value_type::floating) {
        return value_type::floating;
    }
    else {
        return value_type::integer;
    }
}
