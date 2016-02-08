#include "value.hpp"
#include <sstream>

namespace {
    char const *default_string = "";
    constexpr int default_int = -1;
    constexpr gorc::vector<3> default_vector;
    constexpr float default_float = 0.0f;
}

gorc::cog::value::value(deserialization_constructor_tag, binary_input_stream &f)
{
    type_flag = binary_deserialize<value_type>(f);
    f.read(&data, sizeof(data));
}

void gorc::cog::value::binary_serialize_object(binary_output_stream &f) const
{
    binary_serialize(f, type_flag);
    f.write(&data, sizeof(data));
}

gorc::cog::value::value(char const *p)
{
    *this = p;
}

gorc::cog::value& gorc::cog::value::operator=(char const *p)
{
    type_flag = value_type::string;
    data.string = p;
    return *this;
}

gorc::cog::value::operator char const*() const
{
    if(type_flag == value_type::string) {
        return data.string;
    }
    else {
        return default_string;
    }
}

gorc::cog::value::value(bool b)
{
    *this = b;
}

gorc::cog::value& gorc::cog::value::operator=(bool b)
{
    type_flag = value_type::boolean;
    data.boolean = b;
    return *this;
}

gorc::cog::value::operator bool() const
{
    switch(type_flag) {
    case value_type::boolean:
        return data.boolean;

    case value_type::integer:
        return data.integer != 0;

    case value_type::floating:
        return !almost_equal(data.floating, 0.0f);

    case value_type::string:
        return true;

    case value_type::vector:
        return true;

    default:
        return false;
    }
}

gorc::cog::value::value(int i)
{
    *this = i;
}

gorc::cog::value& gorc::cog::value::operator=(int i)
{
    type_flag = value_type::integer;
    data.integer = i;
    return *this;
}

gorc::cog::value::operator int() const
{
    if(is_id_type(type_flag) || type_flag == value_type::integer) {
        return data.integer;
    }
    else if(type_flag == value_type::floating) {
        return static_cast<int>(data.floating);
    }
    else if(type_flag == value_type::boolean) {
        return data.boolean ? 1 : 0;
    }
    else {
        return default_int;
    }
}

gorc::cog::value::value(float f)
{
    *this = f;
}

gorc::cog::value& gorc::cog::value::operator=(float f)
{
    type_flag = value_type::floating;
    data.floating = f;
    return *this;
}

gorc::cog::value::operator float() const
{
    if(is_id_type(type_flag) || type_flag == value_type::integer) {
        return static_cast<float>(data.integer);
    }
    else if(type_flag == value_type::floating) {
        return data.floating;
    }
    else if(type_flag == value_type::boolean) {
        return data.boolean ? 1.0f : 0.0f;
    }
    else {
        return default_float;
    }
}

gorc::cog::value::value(vector<3> const &v)
{
    *this = v;
}

gorc::cog::value& gorc::cog::value::operator=(vector<3> const &v)
{
    type_flag = value_type::vector;
    data.vector.x = get<0>(v);
    data.vector.y = get<1>(v);
    data.vector.z = get<2>(v);
    return *this;
}

gorc::cog::value::operator gorc::vector<3>() const
{
    if(type_flag == value_type::vector) {
        return make_vector(data.vector.x, data.vector.y, data.vector.z);
    }
    else {
        return default_vector;
    }
}

#define MAKE_ID_MEMBERS(x) \
gorc::cog::value::value(x##_id id) \
{ \
    *this = id; \
} \
\
gorc::cog::value& gorc::cog::value::operator=(x##_id id) \
{ \
    type_flag = value_type::x; \
    data.integer = static_cast<int>(id); \
    return *this; \
} \
\
gorc::cog::value::operator gorc::x##_id() const \
{ \
    if(type_flag == value_type::x) { \
        return x##_id(data.integer); \
    } \
    else { \
        return invalid_id; \
    } \
}

MAKE_ID_MEMBERS(sector)
MAKE_ID_MEMBERS(surface)
MAKE_ID_MEMBERS(thing)
MAKE_ID_MEMBERS(player)
MAKE_ID_MEMBERS(ai)
MAKE_ID_MEMBERS(cog)
MAKE_ID_MEMBERS(keyframe)
MAKE_ID_MEMBERS(material)
MAKE_ID_MEMBERS(model)
MAKE_ID_MEMBERS(sound)
MAKE_ID_MEMBERS(thing_template)
MAKE_ID_MEMBERS(message)

#define MAKE_TYPED_NUMERIC_OPERATOR(op) \
gorc::cog::value gorc::cog::value::operator op(value const &v) const \
{ \
    if(type_flag == value_type::floating || v.type_flag == value_type::floating) { \
        return static_cast<float>(*this) op static_cast<float>(v); \
    } \
    else { \
        return static_cast<int>(*this) op static_cast<int>(v); \
    } \
}

#define MAKE_INTEGER_ONLY_OPERATOR(op) \
gorc::cog::value gorc::cog::value::operator op(value const &v) const \
{ \
    return static_cast<int>(*this) op static_cast<int>(v); \
}

MAKE_TYPED_NUMERIC_OPERATOR(+)
MAKE_TYPED_NUMERIC_OPERATOR(-)
MAKE_TYPED_NUMERIC_OPERATOR(*)
MAKE_TYPED_NUMERIC_OPERATOR(/)
MAKE_INTEGER_ONLY_OPERATOR(%)
MAKE_INTEGER_ONLY_OPERATOR(&)
MAKE_INTEGER_ONLY_OPERATOR(|)
MAKE_INTEGER_ONLY_OPERATOR(^)
MAKE_TYPED_NUMERIC_OPERATOR(>)
MAKE_TYPED_NUMERIC_OPERATOR(<)
MAKE_TYPED_NUMERIC_OPERATOR(>=)
MAKE_TYPED_NUMERIC_OPERATOR(<=)

gorc::cog::value gorc::cog::value::operator==(value const &v) const
{
    if(type_flag == value_type::floating || v.type_flag == value_type::floating) {
        return almost_equal(static_cast<float>(*this), static_cast<float>(v));
    }
    else {
        return static_cast<int>(*this) == static_cast<int>(v);
    }
}

gorc::cog::value gorc::cog::value::operator!=(value const &v) const
{
    if(type_flag == value_type::floating || v.type_flag == value_type::floating) {
        return !almost_equal(static_cast<float>(*this), static_cast<float>(v));
    }
    else {
        return static_cast<int>(*this) != static_cast<int>(v);
    }
}

gorc::cog::value gorc::cog::value::operator&&(value const &v) const
{
    return static_cast<bool>(*this) && static_cast<bool>(v);
}

gorc::cog::value gorc::cog::value::operator||(value const &v) const
{
    return static_cast<bool>(*this) || static_cast<bool>(v);
}

gorc::cog::value gorc::cog::value::operator+() const
{
    return *this;
}

gorc::cog::value gorc::cog::value::operator-() const
{
    return value(0) - *this;
}

gorc::cog::value gorc::cog::value::operator!() const
{
    return !static_cast<bool>(*this);
}

gorc::cog::value_type gorc::cog::value::get_type() const
{
    return type_flag;
}

bool gorc::cog::value::is_same(value const &v) const
{
    if(type_flag != v.type_flag) {
        return false;
    }

    switch(type_flag) {
    case value_type::floating:
        return almost_equal(data.floating, v.data.floating);

    case value_type::boolean:
        return data.boolean == v.data.boolean;

    case value_type::string:
        return std::string(data.string) == std::string(v.data.string);

    case value_type::vector:
        return almost_equal(data.vector.x, v.data.vector.x) &&
               almost_equal(data.vector.y, v.data.vector.y) &&
               almost_equal(data.vector.z, v.data.vector.z);

    default:
        return data.integer == v.data.integer;
    }
}

std::string gorc::cog::value::as_string() const
{
    std::stringstream ss;
    ss << gorc::cog::as_string(type_flag) << "(";

    switch(type_flag) {
    case value_type::nothing:
    case value_type::dynamic:
        break;

    case value_type::string:
        ss << data.string;
        break;

    case value_type::floating:
        ss << data.floating;
        break;

    case value_type::boolean:
        ss << std::boolalpha << data.boolean;
        break;

    case value_type::vector:
        ss << data.vector.x
           << ", "
           << data.vector.y
           << ", "
           << data.vector.z;
        break;

    case value_type::message:
    case value_type::integer:
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
        ss << data.integer;
        break;
    }

    ss << ")";
    return ss.str();
}

std::string gorc::cog::as_string(value const &v)
{
    return v.as_string();
}

bool gorc::cog::is_same(value const &u, value const &v)
{
    return u.is_same(v);
}
