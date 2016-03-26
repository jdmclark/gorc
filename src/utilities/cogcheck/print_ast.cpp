#include "print_ast.hpp"
#include "print_ast_visitor.hpp"
#include <iostream>

void gorc::print_ast(cog::ast::translation_unit &tu,
                     cog::script &script)
{
    char const *t = "    ";
    std::cout << std::endl
              << "ABSTRACT SYNTAX TREE"
              << std::endl
              << "FLAGS = " << std::hex << tu.flags->flags << std::dec
              << std::endl
              << "SYMBOLS"
              << std::endl;
    for(auto const &sym : script.symbols) {
        std::cout << as_string(sym.type) << " " << sym.name << std::endl;
        std::cout << t << "index: " << sym.sequence_number << std::endl;
        std::cout << t << "default value: " << as_string(sym.default_value) << std::endl;
        std::cout << t << "local: " << std::boolalpha << sym.local << std::endl;
        std::cout << t << "desc: " << sym.desc << std::endl;
        std::cout << t << "mask: " << std::hex << static_cast<int>(sym.mask) << std::endl;
        std::cout << t << "linkid: " << std::dec << sym.link_id << std::endl;
        std::cout << t << "nolink: " << std::boolalpha << sym.no_link << std::endl;
    }

    std::cout << std::endl
              << "CODE"
              << std::endl;

    print_ast_visitor pav;
    ast_visit(pav, tu.code->code);
}
