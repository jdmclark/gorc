#pragma once

#include <string>
#include <unordered_map>

namespace gorc {

class input_stream;

namespace cog {
namespace ast {
class translation_unit;
class factory;
}

namespace symbols {
class symbol_table;
}

namespace verbs {
class verb_table;
}

namespace vm {
class value;
}

namespace ir {
class printer;
}

namespace stages {

namespace generate_ast {
ast::translation_unit* generate_ast(input_stream& inputFilestream, ast::factory& factory);
}

namespace semantic_analysis {
void semantic_analysis(ast::translation_unit* ast, symbols::symbol_table& symbolTable, const std::unordered_map<std::string, vm::value>& constantTable,
        verbs::verb_table& verbTable);
}

namespace generate_code {
void generate_code(ast::translation_unit* ast, ir::printer& printer, std::unordered_map<std::string, vm::value> const &constant_table);
}
}
}

}
