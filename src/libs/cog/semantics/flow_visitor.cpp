#include "flow_visitor.hpp"
#include "log/log.hpp"

using namespace gorc;
using namespace gorc::cog;

flow_visitor::flow_visitor(script &out_script,
                           seen_label_set const &called_labels,
                           bool inside_dead_code,
                           bool inside_loop,
                           std::string const &inside_label)
    : out_script(out_script)
    , called_labels(called_labels)
    , inside_dead_code(inside_dead_code)
    , seen_dead_code(false)
    , inside_loop(inside_loop)
    , inside_label(inside_label)
{
    return;
}

flow_visitor::~flow_visitor()
{
    end_dead_code();
}

void flow_visitor::add_to_dead_code(diagnostic_context_location const &e)
{
    if(!inside_dead_code) {
        // Current block is not dead. Don't add.
        return;
    }

    if(seen_dead_code) {
        dead_code_location.last_line = e.last_line;
        dead_code_location.last_col = e.last_col;
    }
    else {
        dead_code_location = e;
    }

    seen_dead_code = true;
}

void flow_visitor::end_dead_code()
{
    if(inside_dead_code && seen_dead_code) {
        diagnostic_context dc(dead_code_location);
        LOG_WARNING("dead code");
    }

    inside_dead_code = false;
    seen_dead_code = false;
}

void flow_visitor::check_flow_out(std::string const &out_label)
{
    if(!out_label.empty() && out_label != inside_label) {
        LOG_WARNING(format("label '%s' flows into label '%s'") %
                    out_label %
                    inside_label);
    }
}

void flow_visitor::visit(ast::compound_statement &s)
{
    ast::visit(*this, s.code);
}

void flow_visitor::visit(ast::empty_statement &s)
{
    add_to_dead_code(s.location);
}

void flow_visitor::visit(ast::expression_statement &s)
{
    add_to_dead_code(s.location);
}

void flow_visitor::visit(ast::break_statement &s)
{
    add_to_dead_code(s.location);

    if(inside_loop) {
        // Everything after a break statement is dead code,
        // but only if it's inside a loop
        inside_dead_code = true;
        inside_label.clear();
    }
}

void flow_visitor::visit(ast::return_statement &s)
{
    add_to_dead_code(s.location);

    // Everything after a return statement is dead code
    inside_dead_code = true;
    inside_label.clear();
}

void flow_visitor::visit(ast::call_statement &s)
{
    add_to_dead_code(s.location);
}

void flow_visitor::visit(ast::if_statement &s)
{
    add_to_dead_code(s.location);

    if(!inside_dead_code) {
        flow_visitor dcv(out_script,
                         called_labels,
                         /* inside dead code */ false,
                         /* inside loop */ false,
                         inside_label);
        ast::visit(dcv, *s.code);

        check_flow_out(dcv.inside_label);
    }
}

void flow_visitor::visit(ast::if_else_statement &s)
{
    add_to_dead_code(s.location);

    if(!inside_dead_code) {
        flow_visitor dcv_o(out_script,
                           called_labels,
                           /* inside dead code */ false,
                           /* inside loop */ false,
                           inside_label);
        ast::visit(dcv_o, *s.else_code);

        flow_visitor dcv(out_script,
                         called_labels,
                         /* inside dead code */ false,
                         /* inside loop */ false,
                         inside_label);
        ast::visit(dcv, *s.code);

        check_flow_out(dcv.inside_label);
        check_flow_out(dcv_o.inside_label);

        // If both branches of an if/else end in dead code,
        // then the code following the if/else is also dead
        if(dcv.inside_dead_code && dcv_o.inside_dead_code) {
            inside_dead_code = true;
            inside_label.clear();
        }
    }
}

void flow_visitor::visit(ast::while_statement &s)
{
    add_to_dead_code(s.location);

    if(!inside_dead_code) {
        flow_visitor dcv(out_script,
                         called_labels,
                         /* inside dead code */ false,
                         /* inside loop */ true,
                         inside_label);
        ast::visit(dcv, *s.code);

        check_flow_out(dcv.inside_label);
    }
}

void flow_visitor::visit(ast::do_statement &s)
{
    add_to_dead_code(s.location);

    if(!inside_dead_code) {
        flow_visitor dcv(out_script,
                         called_labels,
                         /* inside dead code */ false,
                         /* inside loop */ true,
                         inside_label);
        ast::visit(dcv, *s.code);

        check_flow_out(dcv.inside_label);
    }
}

void flow_visitor::visit(ast::for_statement &s)
{
    add_to_dead_code(s.location);

    if(!inside_dead_code) {
        flow_visitor dcv(out_script,
                         called_labels,
                         /* inside dead code */ false,
                         /* inside loop */ true,
                         inside_label);
        ast::visit(dcv, *s.code);

        check_flow_out(dcv.inside_label);
    }
}

void flow_visitor::visit(ast::labeled_statement &s)
{
    if(called_labels.find(s.label->value) != called_labels.end()) {
        end_dead_code();
    }

    if(!inside_label.empty()) {
        LOG_WARNING(format("label '%s' flows into label '%s'") %
                    inside_label %
                    s.label->value);
    }

    inside_label = s.label->value;
    ast::visit(*this, *s.code);
}

void flow_visitor::visit(ast::list_node<ast::statement*> &s)
{
    for(auto &stmt : s.elements) {
        ast::visit(*this, *stmt);
    }
}
