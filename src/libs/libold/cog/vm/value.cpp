#include "value.hpp"
#include "libold/cog/constants.hpp"

using namespace gorc::cog::vm;

value::value()
    : type_flag(type::nothing) {
    return;
}

value::value(const value& v) {
    *this = v;
}

const value& value::operator=(const value& v) {
    type_flag = v.type_flag;
    data = v.data;

    return v;
}

value::value(gorc::entity_id v) {
    *this = v;
}

gorc::entity_id value::operator=(gorc::entity_id v) {
    type_flag = type::integer;
    data.integer = static_cast<int>(v);

    return v;
}

value::operator gorc::entity_id() const {
    return entity_id(static_cast<int>(*this));
}

value::value(int v) {
    *this = v;
}

int value::operator=(int v) {
    type_flag = type::integer;
    data.integer = v;

    return v;
}

value::operator int() const {
    switch(type_flag) {
    case type::nothing:
        return constants::default_int;

    case type::integer:
        return data.integer;

    case type::floating:
        return static_cast<int>(data.floating);

    case type::boolean:
        return (data.boolean) ? 1 : 0;

    case type::string:
        return constants::default_int;

    case type::vector:
        return constants::default_int;

    default:
        return constants::default_int;
    }
}

value::value(float v) {
    *this = v;
}

float value::operator=(float v) {
    type_flag = type::floating;
    data.floating = v;

    return v;
}

value::operator float() const {
    switch(type_flag) {
    case type::nothing:
        return constants::default_float;

    case type::integer:
        return static_cast<float>(data.integer);

    case type::floating:
        return data.floating;

    case type::boolean:
        return (data.boolean) ? 1.0f : 0.0f;

    case type::string:
        return constants::default_float;

    case type::vector:
        return constants::default_float;

    default:
        return constants::default_float;
    }
}

value::value(bool v) {
    *this = v;
}

bool value::operator=(bool v) {
    type_flag = type::boolean;
    data.boolean = v;

    return v;
}

value::operator bool() const {
    switch(type_flag) {
    case type::nothing:
        return false;

    case type::integer:
        return data.integer != 0;

    case type::floating:
        return std::not_equal_to<float>()(data.floating, 0.0f);

    case type::boolean:
        return data.boolean;

    case type::string:
        return true;

    case type::vector:
        return true;

    default:
        return false;
    }
}

value::value(const char* v) {
    *this = v;
}

const char* value::operator=(const char* v) {
    type_flag = type::string;
    data.string = v;

    return v;
}

value::operator const char*() const {
    switch(type_flag) {
    case type::nothing:
        return constants::default_string;

    case type::integer:
        return constants::default_string;

    case type::floating:
        return constants::default_string;

    case type::boolean:
        return constants::default_string;

    case type::string:
        return data.string;

    case type::vector:
        return constants::default_string;

    default:
        return constants::default_string;
    }
}

value::value(const vector<3>& v) {
    *this = v;
}

const gorc::vector<3>& value::operator=(const vector<3>& v) {
    type_flag = type::vector;
    data.vector.x = get<0>(v);
    data.vector.y = get<1>(v);
    data.vector.z = get<2>(v);

    return v;
}

value::operator gorc::vector<3>() const {
    switch(type_flag) {
    case type::nothing:
        return make_zero_vector<3, float>();

    case type::integer:
        return make_zero_vector<3, float>();

    case type::floating:
        return make_zero_vector<3, float>();

    case type::boolean:
        return make_zero_vector<3, float>();

    case type::string:
        return make_zero_vector<3, float>();

    case type::vector:
        return make_vector(data.vector.x, data.vector.y, data.vector.z);

    default:
        return make_zero_vector<3, float>();
    }
}
