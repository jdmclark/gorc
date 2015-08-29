#include "symbol_field_visitor.hpp"
#include "log/log.hpp"

namespace gorc {
    namespace cog {
        namespace {

            bool sanity_check(value_type expected_type)
            {
                switch(expected_type) {
                case value_type::integer:
                case value_type::floating:
                case value_type::ai:
                case value_type::keyframe:
                case value_type::material:
                case value_type::model:
                case value_type::sound:
                case value_type::thing_template:
                    // These types accept a default value
                    return true;

                default:
                    LOG_ERROR("symbol does not accept a default value");
                    return false;
                }
            }
        }
    }
}

gorc::cog::symbol_field_visitor::symbol_field_visitor(script &output_script,
                                                      value_type expected_type)
    : output_script(output_script)
    , expected_type(expected_type)
{
    return;
}

gorc::cog::value gorc::cog::symbol_field_visitor::visit(ast::string_fragment_field &v) const
{
    if(!sanity_check(expected_type)) {
        return value();
    }

    switch(expected_type) {
    case value_type::ai:
    case value_type::keyframe:
    case value_type::material:
    case value_type::model:
    case value_type::sound:
    case value_type::thing_template:
        return value(output_script.strings.add_string(v.value));

    default:
        LOG_ERROR("type mismatch");
        return value();
    }
}

gorc::cog::value gorc::cog::symbol_field_visitor::visit(ast::integer_field &v) const
{
    if(!sanity_check(expected_type)) {
        return value();
    }

    if(is_scalar_type(expected_type)) {
        return value(v.value);
    }
    else {
        LOG_ERROR("type mismatch");
        return value();
    }
}

gorc::cog::value gorc::cog::symbol_field_visitor::visit(ast::float_field &v) const
{
    if(!sanity_check(expected_type)) {
        return value();
    }

    if(is_scalar_type(expected_type)) {
        return value(v.value);
    }
    else {
        LOG_ERROR("type mismatch");
        return value();
    }
}
