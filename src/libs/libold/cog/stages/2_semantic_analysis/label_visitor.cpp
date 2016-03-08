#include "label_visitor.hpp"

#include <algorithm>

using namespace gorc::cog::ast;
using gorc::cog::stages::semantic_analysis::label_visitor;

label_visitor::label_visitor(std::unordered_set<std::string>& SeenLabels)
    : SeenLabels(SeenLabels) {
    return;
}

void label_visitor::visit(compound_statement& s) {
    for(auto& stmt : *s.code) {
        stmt->accept(*this);
    }
}

void label_visitor::visit(empty_statement&) {
    return;
}

void label_visitor::visit(expression_statement&) {
    return;
}

void label_visitor::visit(break_statement&) {
    return;
}

void label_visitor::visit(return_statement&) {
    return;
}

void label_visitor::visit(call_statement&) {
    return;
}

void label_visitor::visit(if_statement& s) {
    s.code->accept(*this);
}

void label_visitor::visit(if_else_statement& s) {
    s.code->accept(*this);
    s.else_code->accept(*this);
}

void label_visitor::visit(while_statement& s) {
    s.code->accept(*this);
}

void label_visitor::visit(do_statement& s) {
    s.code->accept(*this);
}

void label_visitor::visit(for_statement& s) {
    s.code->accept(*this);
}

void label_visitor::visit(labeled_statement& s) {
    // Convert label name to lowercase for processing
    std::transform(s.label.begin(), s.label.end(), s.label.begin(), tolower);

    if(SeenLabels.find(s.label) != SeenLabels.end()) {
        LOG_ERROR(format("label '%s' already defined") % s.label);
    }
    else {
        SeenLabels.insert(s.label);
    }

    s.code->accept(*this);
}
