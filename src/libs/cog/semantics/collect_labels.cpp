#include "collect_labels.hpp"
#include "log/log.hpp"

gorc::cog::collect_labels_visitor::collect_labels_visitor(seen_label_set &seen_labels,
                                                          seen_label_set &called_labels)
    : seen_labels(seen_labels)
    , called_labels(called_labels)
{
    return;
}

void gorc::cog::collect_labels_visitor::visit(ast::identifier &id)
{
    bool could_insert = seen_labels.insert(id.value).second;
    if(!could_insert) {
        LOG_ERROR(format("label '%s' already defined") % id.value);
    }
}

void gorc::cog::collect_labels_visitor::visit(ast::list_node<ast::statement*> &statements)
{
    for(auto &stmt : statements.elements) {
        ast::visit(*this, *stmt);
    }
}

void gorc::cog::collect_labels_visitor::visit(ast::compound_statement &s)
{
    ast::visit(*this, s.code);
}

void gorc::cog::collect_labels_visitor::visit(ast::empty_statement &)
{
    return;
}

void gorc::cog::collect_labels_visitor::visit(ast::expression_statement &)
{
    return;
}

void gorc::cog::collect_labels_visitor::visit(ast::break_statement &)
{
    return;
}

void gorc::cog::collect_labels_visitor::visit(ast::return_statement &)
{
    return;
}

void gorc::cog::collect_labels_visitor::visit(ast::call_statement &s)
{
    called_labels.insert(s.label->value);
    return;
}

void gorc::cog::collect_labels_visitor::visit(ast::if_statement &s)
{
    ast::visit(*this, *s.code);
}

void gorc::cog::collect_labels_visitor::visit(ast::if_else_statement &s)
{
    ast::visit(*this, *s.code);
    ast::visit(*this, *s.else_code);
}

void gorc::cog::collect_labels_visitor::visit(ast::while_statement &s)
{
    ast::visit(*this, *s.code);
}

void gorc::cog::collect_labels_visitor::visit(ast::do_statement &s)
{
    ast::visit(*this, *s.code);
}

void gorc::cog::collect_labels_visitor::visit(ast::for_statement &s)
{
    ast::visit(*this, *s.code);
}

void gorc::cog::collect_labels_visitor::visit(ast::labeled_statement &s)
{
    ast::visit(*this, s.label);
    ast::visit(*this, *s.code);
}
