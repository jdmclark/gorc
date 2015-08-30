#include "code_visitor.hpp"
#include "rvalue_visitor.hpp"
#include "variant_location_visitor.hpp"
#include "for_expression_visitor.hpp"
#include "log/log.hpp"

using namespace gorc;
using namespace gorc::cog;

code_visitor::code_visitor(script &out_script,
                           ast::factory &factory,
                           constant_table const &constants,
                           verb_table const &verbs,
                           seen_label_set const &labels,
                           bool inside_loop,
                           bool inside_block)
    : out_script(out_script)
    , factory(factory)
    , constants(constants)
    , verbs(verbs)
    , labels(labels)
    , inside_loop(inside_loop)
    , inside_block(inside_block)
{
    return;
}

void code_visitor::visit(ast::compound_statement &s)
{
    ast::visit(*this, s.code);
}

void code_visitor::visit(ast::empty_statement &)
{
    return;
}

void code_visitor::visit(ast::expression_statement &s)
{
    rvalue_visitor rvv(out_script,
                       factory,
                       constants,
                       verbs,
                       /* result is used */ false);
    visit_and_fold(rvv, *s.value, factory);
}

void code_visitor::visit(ast::break_statement &)
{
    if(!inside_loop) {
        LOG_ERROR("break used outside loop");
    }
}

void code_visitor::visit(ast::return_statement &)
{
    return;
}

void code_visitor::visit(ast::call_statement &s)
{
    if(labels.find(s.label->value) == labels.end()) {
        LOG_ERROR(format("label '%s' called but not defined") % s.label->value);
    }
}

void code_visitor::visit(ast::if_statement &s)
{
    visit_and_fold_boolean_condition(*s.condition,
                                     out_script,
                                     factory,
                                     constants,
                                     verbs);

    code_visitor cv(out_script,
                    factory,
                    constants,
                    verbs,
                    labels,
                    inside_loop,
                    /* inside block */ true);
    ast::visit(cv, *s.code);
}

void code_visitor::visit(ast::if_else_statement &s)
{
    visit_and_fold_boolean_condition(*s.condition,
                                     out_script,
                                     factory,
                                     constants,
                                     verbs);

    code_visitor cv(out_script,
                    factory,
                    constants,
                    verbs,
                    labels,
                    inside_loop,
                    /* inside block */ true);
    ast::visit(cv, *s.code);
    ast::visit(cv, *s.else_code);
}

void code_visitor::visit(ast::while_statement &s)
{
    visit_and_fold_boolean_condition(*s.condition,
                                     out_script,
                                     factory,
                                     constants,
                                     verbs);

    code_visitor cv(out_script,
                    factory,
                    constants,
                    verbs,
                    labels,
                    /* inside loop */ true,
                    /* inside block */ true);
    ast::visit(cv, *s.code);
}

void code_visitor::visit(ast::do_statement &s)
{
    visit_and_fold_boolean_condition(*s.condition,
                                     out_script,
                                     factory,
                                     constants,
                                     verbs);

    code_visitor cv(out_script,
                    factory,
                    constants,
                    verbs,
                    labels,
                    /* inside loop */ true,
                    /* inside block */ true);
    ast::visit(cv, *s.code);
}

void code_visitor::visit(ast::for_statement &s)
{
    for_expression_visitor fev(out_script,
                               factory,
                               constants,
                               verbs,
                               /* result is used */ false);
    for_expression_visitor few(out_script,
                               factory,
                               constants,
                               verbs,
                               /* result is used */ true);

    ast::visit(fev, *s.initializer);
    auto cond_type = ast::visit(few, *s.condition);
    if(!is_truth_value_type(cond_type)) {
        diagnostic_context dc(ast::visit(variant_location_visitor(), *s.condition));
        LOG_WARNING(format("implicit cast from %s to bool used in condition") %
                    as_string(cond_type));
    }
    ast::visit(fev, *s.incrementer);

    code_visitor cv(out_script,
                    factory,
                    constants,
                    verbs,
                    labels,
                    /* inside loop */ true,
                    /* inside block */ true);
    ast::visit(cv, *s.code);
}

void code_visitor::visit(ast::labeled_statement &s)
{
    if(inside_block) {
        diagnostic_context dc(s.label->location);
        LOG_WARNING(format("label '%s' is defined inside block") % s.label->value);
    }

    ast::visit(*this, *s.code);
}

void code_visitor::visit(ast::list_node<ast::statement*> &s)
{
    for(auto &stmt : s.elements) {
        ast::visit(*this, *stmt);
    }
}
