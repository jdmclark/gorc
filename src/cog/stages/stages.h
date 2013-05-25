#pragma once

#include <string>
#include <unordered_map>

namespace Gorc {

namespace Text {
class Source;
}

namespace Diagnostics {
class Report;
}

namespace Cog {
namespace AST {
class TranslationUnit;
class Factory;
}

namespace Symbols {
class SymbolTable;
}

namespace Verbs {
class VerbTable;
}

namespace VM {
class Value;
}

namespace IR {
class Printer;
}

namespace Stages {

namespace GenerateAST {
AST::TranslationUnit* GenerateAST(Text::Source& inputFilestream, Diagnostics::Report& report, AST::Factory& factory);
}

namespace SemanticAnalysis {
void SemanticAnalysis(AST::TranslationUnit* ast, Symbols::SymbolTable& symbolTable, const std::unordered_map<std::string, VM::Value>& constantTable,
		Verbs::VerbTable& verbTable, Diagnostics::Report& report);
}

namespace ConstantFolding {
void ConstantFolding(AST::Factory& factory, AST::TranslationUnit* ast, Symbols::SymbolTable& symbolTable,
		const std::unordered_map<std::string, VM::Value>& constantTable, Diagnostics::Report& report);
}

namespace GenerateCode {
void GenerateCode(AST::TranslationUnit* ast, IR::Printer& printer, Diagnostics::Report& report);
}
}
}

}
