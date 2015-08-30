#include "rvalue_visitor.hpp"
#include "lvalue_visitor.hpp"
#include "utility/zip.hpp"
#include "variant_location_visitor.hpp"

using namespace gorc;
using namespace gorc::cog;

rvalue_visitor::rvalue_visitor(script &out_script,
                               ast::factory &factory,
                               constant_table const &constants,
                               verb_table const &verbs,
                               bool result_is_used)
    : out_script(out_script)
    , factory(factory)
    , constants(constants)
    , verbs(verbs)
    , result_is_used(result_is_used)
{
    return;
}

// The following function cannot be called during normal execution.
// Immediate expressions are produced by rvalue_visitor during constant folding.
// LCOV_EXCL_START
expr_val rvalue_visitor::visit(ast::immediate_expression &e)
{
    return expr_val(e.value.get_type(), e.value);
}
// LCOV_EXCL_STOP

expr_val rvalue_visitor::visit(ast::string_literal_expression &e)
{
    return expr_val(value_type::string,
                    value(out_script.strings.add_string(e.value)));
}

expr_val rvalue_visitor::visit(ast::integer_literal_expression &e)
{
    return expr_val(value_type::integer,
                    value(e.value));
}

expr_val rvalue_visitor::visit(ast::float_literal_expression &e)
{
    return expr_val(value_type::floating,
                    value(e.value));
}

expr_val rvalue_visitor::visit(ast::vector_literal_expression &e)
{
    return expr_val(value_type::vector,
                    value(make_vector(e.x, e.y, e.z)));
}

expr_val rvalue_visitor::visit(ast::identifier_expression &e)
{
    // Check constant table first
    auto it = constants.find(e.value->value);
    if(it != constants.end()) {
        // Identifier is a constant
        return expr_val(it->second.get_type(), it->second);
    }

    // Identifier is not a constant
    auto const &sym = out_script.symbols.get_symbol(e.value->value);
    return expr_val(sym.type, nothing);
}

expr_val rvalue_visitor::visit(ast::subscript_expression &e)
{
    visit_and_fold_array_subscript(*e.index,
                                   out_script,
                                   factory,
                                   constants,
                                   verbs);

    // Look up base symbol, and assume indexed symbol shares type
    auto const &sym = out_script.symbols.get_symbol(e.base->value);
    return expr_val(sym.type, nothing);
}

expr_val rvalue_visitor::visit(ast::method_call_expression &e)
{
    verb_id vid;

    try {
        vid = verbs.get_verb_id(e.base->value);
    }
    catch(logged_runtime_error const &e) {
        // Verb does not exist, which is fatal. Return fake constexpr.
        return expr_val(value_type::dynamic, nothing);
    }

    verb const &v = verbs.get_verb(vid);

    if(e.arguments->elements.size() != v.argument_types.size()) {
        LOG_ERROR(format("verb '%s' expects %d arguments, found %d") %
                  v.name %
                  v.argument_types.size() %
                  e.arguments->elements.size());
        return expr_val(v.return_type, nothing);
    }

    rvalue_visitor rv(out_script,
                      factory,
                      constants,
                      verbs,
                      /* value is used */ true);
    for(auto const &arg_pair : zip(v.argument_types, e.arguments->elements)) {
        auto folded_type = visit_and_fold(rv, *std::get<1>(arg_pair), factory);
        if(!can_convert_type(folded_type, std::get<0>(arg_pair))) {
            diagnostic_context dc(ast::visit(variant_location_visitor(), *std::get<1>(arg_pair)));
            LOG_WARNING(format("cannot convert argument from %s to %s") %
                        as_string(folded_type) %
                        as_string(std::get<0>(arg_pair)));
        }
    }

    return expr_val(v.return_type, nothing);
}

expr_val rvalue_visitor::visit(ast::unary_expression &e)
{
    rvalue_visitor rv(out_script,
                      factory,
                      constants,
                      verbs,
                      /* value used */ true);

    auto sub_val = ast::visit(rv, *e.base);
    auto sub_type = std::get<0>(sub_val);
    auto return_type = sub_type;

    switch(e.op) {
    case ast::unary_operator::plus:
    case ast::unary_operator::minus:
        return_type = get_closest_numeric_type(sub_type);
        if(!can_convert_type(sub_type, return_type)) {
            LOG_WARNING("numeric operator used with non-numeric value");
        }
        break;

    case ast::unary_operator::logical_not:
        if(!is_truth_value_type(sub_type)) {
            LOG_WARNING(format("implicit cast from %s to bool used in condition") %
                        as_string(sub_type));
        }
        return_type = value_type::boolean;
        break;
    }

    return maybe_if(std::get<1>(sub_val),
                    expr_val(return_type, nothing),
                    [&](value v) {
            switch(e.op) {
            case ast::unary_operator::plus:
                return expr_val(v.get_type(), v);
            case ast::unary_operator::minus:
                return expr_val(v.get_type(), -v);
            case ast::unary_operator::logical_not:
                return expr_val(value_type::boolean, !v);
            }

            // Catch-all: the above switch statement should handle all cases
            LOG_FATAL("unhandled unary operator"); // LCOV_EXCL_LINE
        });
}

expr_val rvalue_visitor::visit(ast::infix_expression &e)
{
    rvalue_visitor rv(out_script,
                      factory,
                      constants,
                      verbs,
                      /* value used */ true);

    auto left = ast::visit(rv, *e.left);
    auto left_type = std::get<0>(left);

    auto right = ast::visit(rv, *e.right);
    auto right_type = std::get<0>(right);

    auto common_type = get_common_numeric_type(left_type, right_type);

    // Check input types
    switch(e.op) {
    case ast::infix_operator::logical_and:
    case ast::infix_operator::logical_or:
        if(!is_truth_value_type(left_type)) {
            diagnostic_context dc(ast::visit(variant_location_visitor(), *e.left));
            LOG_WARNING(format("implicit cast from %s to bool used in condition") %
                        as_string(left_type));
        }

        if(!is_truth_value_type(right_type)) {
            diagnostic_context dc(ast::visit(variant_location_visitor(), *e.right));
            LOG_WARNING(format("implicit cast from %s to bool used in condition") %
                        as_string(right_type));
        }
        break;

    case ast::infix_operator::equal:
    case ast::infix_operator::not_equal:
        if(is_id_type(left_type) && is_id_type(right_type)) {
            if(left_type != value_type::dynamic &&
               right_type != value_type::dynamic &&
               left_type != right_type) {
                LOG_WARNING(format("comparison between %s and %s ids") %
                            as_string(left_type) %
                            as_string(right_type));
            }
            break;
        }

        // Deliberate fall-through
    default:
        if(!can_convert_type(left_type, common_type) ||
           !can_convert_type(right_type, common_type)) {
            LOG_WARNING(format("used numeric operator on incompatible types %s and %s") %
                        as_string(left_type) %
                        as_string(right_type));
        }
        break;
    }

    value_type result_type;
    switch(e.op) {
    case ast::infix_operator::addition:
    case ast::infix_operator::subtraction:
    case ast::infix_operator::multiplication:
    case ast::infix_operator::division:
        // Result is lowest common denominator:
        result_type = common_type;
        break;

    case ast::infix_operator::modulo:
    case ast::infix_operator::bitwise_and:
    case ast::infix_operator::bitwise_xor:
    case ast::infix_operator::bitwise_or:
        result_type = value_type::integer;
        break;

    case ast::infix_operator::greater:
    case ast::infix_operator::greater_equal:
    case ast::infix_operator::less:
    case ast::infix_operator::less_equal:
    case ast::infix_operator::equal:
    case ast::infix_operator::not_equal:
    case ast::infix_operator::logical_and:
    case ast::infix_operator::logical_or:
        result_type = value_type::boolean;
        break;
    }

    if(std::get<1>(left).has_value() && std::get<1>(right).has_value()) {
        auto left_value = std::get<1>(left).get_value();
        auto right_value = std::get<1>(right).get_value();

        value result;
        switch(e.op) {
        case ast::infix_operator::addition:
            result = left_value + right_value;
            break;

        case ast::infix_operator::subtraction:
            result = left_value - right_value;
            break;

        case ast::infix_operator::multiplication:
            result = left_value * right_value;
            break;

        case ast::infix_operator::division:
            result = left_value / right_value;
            break;

        case ast::infix_operator::modulo:
            result = left_value % right_value;
            break;

        case ast::infix_operator::greater:
            result = left_value > right_value;
            break;

        case ast::infix_operator::greater_equal:
            result = left_value >= right_value;
            break;

        case ast::infix_operator::less:
            result = left_value < right_value;
            break;

        case ast::infix_operator::less_equal:
            result = left_value <= right_value;
            break;

        case ast::infix_operator::equal:
            result = left_value == right_value;
            break;

        case ast::infix_operator::not_equal:
            result = left_value != right_value;
            break;

        case ast::infix_operator::bitwise_and:
            result = left_value & right_value;
            break;

        case ast::infix_operator::bitwise_or:
            result = left_value | right_value;
            break;

        case ast::infix_operator::bitwise_xor:
            result = left_value ^ right_value;
            break;

        case ast::infix_operator::logical_and:
            result = left_value && right_value;
            break;

        case ast::infix_operator::logical_or:
            result = left_value || right_value;
            break;
        }

        return expr_val(result.get_type(), result);
    }

    // Either the left or right could still need replacement
    maybe_if(std::get<1>(left), [&](value v) {
            auto loc = ast::visit(variant_location_visitor(), *e.left);
            *e.left = factory.make<ast::immediate_expression>(loc, v);
        });
    maybe_if(std::get<1>(right), [&](value v) {
            auto loc = ast::visit(variant_location_visitor(), *e.right);
            *e.right = factory.make<ast::immediate_expression>(loc, v);
        });

    return expr_val(result_type, nothing);
}

expr_val rvalue_visitor::visit(ast::assignment_expression &e)
{
    if(result_is_used) {
        LOG_WARNING("result of an assignment expression is used: behavior differs from "
                    "original implementation");
    }

    // The result of the right side is always used for assignment.
    rvalue_visitor rv(out_script,
                      factory,
                      constants,
                      verbs,
                      /* value is used */ true);
    auto val = visit_and_fold(rv, *e.value, factory);
    lvalue_visitor lv(out_script, factory, constants, verbs);
    auto lval_type = ast::visit(lv, *e.target);

    // Check that this lvalue can accept the value
    if(!can_convert_type(val, lval_type)) {
        LOG_WARNING(format("assignment changes type of lvalue from %s to %s") %
                    as_string(lval_type) %
                    as_string(val));
    }

    return expr_val(val, nothing);
}

expr_val rvalue_visitor::visit(ast::comma_expression &e)
{
    // The value of the left side is never used
    rvalue_visitor rv(out_script,
                      factory,
                      constants,
                      verbs,
                      /* value is used */ false);
    auto left = ast::visit(rv, *e.left);
    auto right = ast::visit(*this, *e.right);
    if(std::get<1>(left).has_value() && std::get<1>(right).has_value()) {
        // This is a constant comma expression
        return right;
    }

    // Either the left or right could still need replacement
    maybe_if(std::get<1>(left), [&](value v) {
            auto loc = ast::visit(variant_location_visitor(), *e.left);
            *e.left = factory.make<ast::immediate_expression>(loc, v);
        });
    maybe_if(std::get<1>(right), [&](value v) {
            auto loc = ast::visit(variant_location_visitor(), *e.right);
            *e.right = factory.make<ast::immediate_expression>(loc, v);
        });

    return expr_val(std::get<0>(right), nothing);
}

void gorc::cog::visit_and_fold_boolean_condition(ast::expression &e,
                                                 script &out_script,
                                                 ast::factory &factory,
                                                 constant_table const &constants,
                                                 verb_table const &verbs)
{
    rvalue_visitor rvv(out_script,
                       factory,
                       constants,
                       verbs,
                       /* result is used */ true);
    auto cond_type = visit_and_fold(rvv, e, factory);
    if(!is_truth_value_type(cond_type)) {
        diagnostic_context dc(ast::visit(variant_location_visitor(), e));
        LOG_WARNING(format("implicit cast from %s to bool used in condition") %
                    as_string(cond_type));
    }
}

void gorc::cog::visit_and_fold_array_subscript(ast::expression &e,
                                               script &out_script,
                                               ast::factory &factory,
                                               constant_table const &constants,
                                               verb_table const &verbs)
{
    rvalue_visitor rvv(out_script,
                       factory,
                       constants,
                       verbs,
                       /* result is used*/ true);
    auto cond_type = visit_and_fold(rvv, e, factory);
    if(!is_scalar_type(cond_type)) {
        diagnostic_context dc(ast::visit(variant_location_visitor(), e));
        LOG_ERROR("array subscript is not numeric");
    }
}
