#include "print_ast_visitor.hpp"
#include <iostream>

using namespace gorc;
using namespace gorc::cog;

void print_ast_visitor::print_tabs()
{
    for(int i = 0; i < indentation; ++i) {
        std::cout << "  ";
    }
}

void print_ast_visitor::visit(ast::list_node<ast::statement*> &s)
{
    for(auto const &stmt : s.elements) {
        ast::visit(*this, *stmt);
    }
}

void print_ast_visitor::visit(ast::compound_statement &s)
{
    print_tabs();
    std::cout << "block:" << std::endl;
    ++indentation;
    ast::visit(*this, s.code);
    --indentation;
}

void print_ast_visitor::visit(ast::empty_statement &)
{
    print_tabs();
    std::cout << ";" << std::endl;
}

void print_ast_visitor::visit(ast::expression_statement &s)
{
    print_tabs();
    ast::visit(*this, *s.value);
    std::cout << ";" << std::endl;
}

void print_ast_visitor::visit(ast::break_statement &)
{
    print_tabs();
    std::cout << "break;" << std::endl;
}

void print_ast_visitor::visit(ast::return_statement &)
{
    print_tabs();
    std::cout << "return;" << std::endl;
}

void print_ast_visitor::visit(ast::call_statement &s)
{
    print_tabs();
    std::cout << "call " << s.label->value << ";" << std::endl;
}

void print_ast_visitor::visit(ast::if_statement &s)
{
    print_tabs();
    std::cout << "if ";
    ast::visit(*this, *s.condition);
    std::cout << ":" << std::endl;
    ++indentation;
    ast::visit(*this, *s.code);
    --indentation;
}

void print_ast_visitor::visit(ast::if_else_statement &s)
{
    print_tabs();
    std::cout << "if ";
    ast::visit(*this, *s.condition);
    std::cout << ":" << std::endl;
    ++indentation;
    ast::visit(*this, *s.code);
    --indentation;
    print_tabs();
    std::cout << "else:" << std::endl;
    ++indentation;
    ast::visit(*this, *s.else_code);
    --indentation;
}

void print_ast_visitor::visit(ast::while_statement &s)
{
    print_tabs();
    std::cout << "while ";
    ast::visit(*this, *s.condition);
    std::cout << ":" << std::endl;
    ++indentation;
    ast::visit(*this, *s.code);
    --indentation;
}

void print_ast_visitor::visit(ast::do_statement &s)
{
    print_tabs();
    std::cout << "dowhile ";
    ast::visit(*this, *s.condition);
    std::cout << ":" << std::endl;
    ++indentation;
    ast::visit(*this, *s.code);
    --indentation;
}

void print_ast_visitor::visit(ast::for_statement &s)
{
    print_tabs();
    std::cout << "for ";
    ast::visit(*this, *s.initializer);
    std::cout << "; ";
    ast::visit(*this, *s.condition);
    std::cout << "; ";
    ast::visit(*this, *s.incrementer);
    std::cout << ":" << std::endl;
    ++indentation;
    ast::visit(*this, *s.code);
    --indentation;
}

void print_ast_visitor::visit(ast::labeled_statement &s)
{
    print_tabs();
    std::cout << s.label->value << ":" << std::endl;
    ast::visit(*this, *s.code);
}


void print_ast_visitor::visit(ast::immediate_expression &s)
{
    std::cout << as_string(s.value);
}

void print_ast_visitor::visit(ast::string_literal_expression &s)
{
    std::cout << s.value;
}

void print_ast_visitor::visit(ast::integer_literal_expression &s)
{
    std::cout << s.value;
}

void print_ast_visitor::visit(ast::float_literal_expression &s)
{
    std::cout << s.value;
}

void print_ast_visitor::visit(ast::vector_literal_expression &s)
{
    std::cout << "'" << s.x << ", " << s.y << ", " << s.z << "'";
}

void print_ast_visitor::visit(ast::identifier_expression &s)
{
    std::cout << s.value->value;
}

void print_ast_visitor::visit(ast::subscript_expression &s)
{
    std::cout << s.base->value << "[";
    ast::visit(*this, *s.index);
    std::cout << "]";
}

void print_ast_visitor::visit(ast::method_call_expression &s)
{
    std::cout << "(call " << s.base->value;
    for(auto const &arg : s.arguments->elements) {
        std::cout << " ";
        ast::visit(*this, *arg);
    }
    std::cout << ")";
}

void print_ast_visitor::visit(ast::unary_expression &s)
{
    switch(s.op) {
    case ast::unary_operator::plus:
        std::cout << "(+ ";
        break;

    case ast::unary_operator::minus:
        std::cout << "(- ";
        break;

    case ast::unary_operator::logical_not:
        std::cout << "(! ";
        break;
    }

    ast::visit(*this, *s.base);
    std::cout << ")";
}

void print_ast_visitor::visit(ast::infix_expression &s)
{
    switch(s.op) {
    case ast::infix_operator::addition:
        std::cout << "(+ ";
        break;

    case ast::infix_operator::subtraction:
        std::cout << "(- ";
        break;

    case ast::infix_operator::multiplication:
        std::cout << "(* ";
        break;

    case ast::infix_operator::division:
        std::cout << "(/ ";
        break;

    case ast::infix_operator::modulo:
        std::cout << "(% ";
        break;

    case ast::infix_operator::greater:
        std::cout << "(> ";
        break;

    case ast::infix_operator::greater_equal:
        std::cout << "(>= ";
        break;

    case ast::infix_operator::less:
        std::cout << "(< ";
        break;

    case ast::infix_operator::less_equal:
        std::cout << "(<= ";
        break;

    case ast::infix_operator::equal:
        std::cout << "(== ";
        break;

    case ast::infix_operator::not_equal:
        std::cout << "(!= ";
        break;

    case ast::infix_operator::bitwise_or:
        std::cout << "(| ";
        break;

    case ast::infix_operator::bitwise_and:
        std::cout << "(& ";
        break;

    case ast::infix_operator::bitwise_xor:
        std::cout << "(^ ";
        break;

    case ast::infix_operator::logical_and:
        std::cout << "(&& ";
        break;

    case ast::infix_operator::logical_or:
        std::cout << "(|| ";
        break;
    }

    ast::visit(*this, *s.left);
    std::cout << " ";
    ast::visit(*this, *s.right);
    std::cout << ")";
}

void print_ast_visitor::visit(ast::assignment_expression &s)
{
    std::cout << "(";
    ast::visit(*this, *s.target);
    std::cout << " <- ";
    ast::visit(*this, *s.value);
    std::cout << ")";
}

void print_ast_visitor::visit(ast::comma_expression &s)
{
    std::cout << "(";
    ast::visit(*this, *s.left);
    std::cout << " ";
    ast::visit(*this, *s.right);
    std::cout << ")";
}


void print_ast_visitor::visit(ast::for_empty_expression &)
{
    return;
}

void print_ast_visitor::visit(ast::for_expression &s)
{
    ast::visit(*this, *s.condition);
}

