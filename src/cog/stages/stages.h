#pragma once

#include <string>
#include <unordered_map>

namespace gorc {

namespace text {
class source;
}

namespace diagnostics {
class report;
}

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
ast::translation_unit* generate_ast(text::source& inputFilestream, diagnostics::report& report, ast::factory& factory);
}

namespace semantic_analysis {
void semantic_analysis(ast::translation_unit* ast, symbols::symbol_table& symbolTable, const std::unordered_map<std::string, vm::value>& constantTable,
		verbs::verb_table& verbTable, diagnostics::report& report);
}

namespace constant_folding {
void constant_folding(ast::factory& factory, ast::translation_unit* ast, symbols::symbol_table& symbolTable,
		const std::unordered_map<std::string, vm::value>& constantTable, diagnostics::report& report);
}

namespace generate_code {
void generate_code(ast::translation_unit* ast, ir::printer& printer, diagnostics::report& report);
}
}
}

}
