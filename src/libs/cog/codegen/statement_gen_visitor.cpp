#include "statement_gen_visitor.hpp"
#include "nonval_expression_gen_visitor.hpp"
#include "rval_expression_gen_visitor.hpp"
#include "log/log.hpp"

using namespace gorc;
using namespace gorc::cog;

statement_gen_visitor::statement_gen_visitor(script &out_script,
                                             ir_printer &ir,
                                             verb_table const &verbs,
                                             constant_table const &constants)
    : out_script(out_script)
    , ir(ir)
    , verbs(verbs)
    , constants(constants)
{
    return;
}

void statement_gen_visitor::visit(ast::compound_statement &s)
{
    ast::visit(*this, s.code);
}

void statement_gen_visitor::visit(ast::empty_statement &)
{
    return;
}

void statement_gen_visitor::visit(ast::expression_statement &s)
{
    nonval_expression_gen_visitor ev(out_script, ir, verbs, constants);
    ast::visit(ev, *s.value);
}

void statement_gen_visitor::visit(ast::break_statement &)
{
    if(break_labels.empty()) {
        LOG_FATAL("generated break instruction outside of loop");
    }

    ir.jmp(break_labels.top());
}

void statement_gen_visitor::visit(ast::return_statement &)
{
    ir.ret();
}

void statement_gen_visitor::visit(ast::call_statement &s)
{
    ir.jal(ir.get_named_label(s.label->value));
}

void statement_gen_visitor::visit(ast::if_statement &s)
{
    auto after_code_label = ir.generate_label();

    // Push condition onto stack
    rval_expression_gen_visitor ev(out_script, ir, verbs, constants);
    ast::visit(ev, *s.condition);

    // Skip body if condition is false
    ir.bf(after_code_label);

    // Body code
    ast::visit(*this, *s.code);

    // Set label after body
    ir.label(after_code_label);
}

void statement_gen_visitor::visit(ast::if_else_statement &s)
{
    auto after_code_label = ir.generate_label();
    auto after_else_label = ir.generate_label();

    // Push condition onto stack
    rval_expression_gen_visitor ev(out_script, ir, verbs, constants);
    ast::visit(ev, *s.condition);

    // Skip to else if condition is false
    ir.bf(after_code_label);

    // Body code
    ast::visit(*this, *s.code);

    // Skip to end of statement after body
    ir.jmp(after_else_label);

    // Set else label
    ir.label(after_code_label);

    // Else code
    ast::visit(*this, *s.else_code);

    // Set end of statement label
    ir.label(after_else_label);
}

void statement_gen_visitor::visit(ast::while_statement &s)
{
    auto body_label = ir.generate_label();
    auto break_label = ir.generate_label();

    break_labels.push(break_label);

    // Push condition onto stack
    rval_expression_gen_visitor ev(out_script, ir, verbs, constants);
    ast::visit(ev, *s.condition);

    // Skip body if condition is false
    ir.bf(break_label);

    // Insert body label before code
    ir.label(body_label);

    // Body code
    ast::visit(*this, *s.code);

    // Evaluate condition again
    ast::visit(ev, *s.condition);

    // Return to start of body if condition is true
    ir.bt(body_label);

    // Set break label after loop
    ir.label(break_label);

    break_labels.pop();
}

void statement_gen_visitor::visit(ast::do_statement &s)
{
    auto body_label = ir.generate_label();
    auto break_label = ir.generate_label();

    break_labels.push(break_label);

    // Insert body label before code
    ir.label(body_label);

    // Body code
    ast::visit(*this, *s.code);

    // Evaluate condition
    rval_expression_gen_visitor ev(out_script, ir, verbs, constants);
    ast::visit(ev, *s.condition);

    // Return to start of body if condition is true
    ir.bt(body_label);

    // Set break label after loop
    ir.label(break_label);

    break_labels.pop();
}

void statement_gen_visitor::visit(ast::for_statement &s)
{
    auto body_label = ir.generate_label();
    auto break_label = ir.generate_label();

    break_labels.push(break_label);

    // Run initializer code
    nonval_expression_gen_visitor nev(out_script, ir, verbs, constants);
    ast::visit(nev, *s.initializer);

    // Run first condition
    rval_expression_gen_visitor rev(out_script, ir, verbs, constants);
    ast::visit(rev, *s.condition);

    // Escape out of loop if condition is false
    ir.bf(break_label);

    // Insert body label before start of code
    ir.label(body_label);

    // Body code
    ast::visit(*this, *s.code);

    // Incrementation code
    ast::visit(nev, *s.incrementer);

    // Evaluate condition again
    ast::visit(rev, *s.condition);

    // Return to start of loop if condition is true
    ir.bt(body_label);

    // Set break label after loop
    ir.label(break_label);

    break_labels.pop();
}

void statement_gen_visitor::visit(ast::labeled_statement &s)
{
    // Check for exported message
    auto sym = out_script.symbols.get_existing_symbol(s.label->value);
    auto const_it = constants.find(s.label->value);
    if(const_it != constants.end() &&
       sym.has_value() &&
       sym.get_value()->type == value_type::message) {
        // Symbol is an exported message
        ir.label(ir.get_export_label(message_type(static_cast<int>(const_it->second)),
                                     s.label->value));
    }
    else {
        // Symbol is not exported
        ir.label(ir.get_named_label(s.label->value));
    }

    // Generate code for labeled statement
    ast::visit(*this, *s.code);
}

void statement_gen_visitor::visit(ast::list_node<ast::statement*> &s)
{
    for(auto &stmt : s.elements) {
        ast::visit(*this, *stmt);
    }
}
