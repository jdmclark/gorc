#include "function_verb.hpp"

gorc::maybe<gorc::cog::value> gorc::cog::detail::function_verb_type_check(
        stack const &stk,
        verb const &v,
        value default_value)
{
    int argn = static_cast<int>(v.argument_types.size());
    auto type_it = v.argument_types.rbegin();
    bool failed_any = false;
    for(auto stack_it = stk.rbegin();
        type_it != v.argument_types.rend() && stack_it != stk.rend();
        ++type_it, ++stack_it, --argn) {
        bool failed_once = false;
        switch(*type_it) {
        case value_type::sector: {
                sector_id sid = *stack_it;
                failed_once = !sid.is_valid();
            }
            break;

        case value_type::surface: {
                surface_id sid = *stack_it;
                failed_once = !sid.is_valid();
            }
            break;

        case value_type::thing: {
                thing_id sid = *stack_it;
                failed_once = !sid.is_valid();
            }
            break;

        case value_type::player: {
                player_id sid = *stack_it;
                failed_once = !sid.is_valid();
            }
            break;

        case value_type::ai: {
                ai_id sid = *stack_it;
                failed_once = !sid.is_valid();
            }
            break;

        case value_type::cog: {
                cog_id sid = *stack_it;
                failed_once = !sid.is_valid();
            }
            break;

        case value_type::colormap: {
                colormap_id sid = *stack_it;
                failed_once = !sid.is_valid();
            }
            break;

        case value_type::keyframe: {
                keyframe_id sid = *stack_it;
                failed_once = !sid.is_valid();
            }
            break;

        case value_type::material: {
                material_id sid = *stack_it;
                failed_once = !sid.is_valid();
            }
            break;

        case value_type::model: {
                model_id sid = *stack_it;
                failed_once = !sid.is_valid();
            }
            break;

        case value_type::sound: {
                sound_id sid = *stack_it;
                failed_once = !sid.is_valid();
            }
            break;

        case value_type::thing_template: {
                thing_template_id sid = *stack_it;
                failed_once = !sid.is_valid();
            }
            break;

        case value_type::message: {
                message_id sid = *stack_it;
                failed_once = !sid.is_valid();
            }
            break;

        default:
            // Other types are always safely cast
            failed_once = false;
        }

        if(failed_once) {
            failed_any = true;
            LOG_ERROR(format("could not convert argument %d from %s to %s") %
                      argn %
                      as_string(*stack_it) %
                      as_string(*type_it));
        }
    }

    if(failed_any) {
        return default_value;
    }
    else {
        return nothing;
    }
}
