#pragma once

#include <string>
#include "cog/ast/factory.h"
#include "framework/text/source.h"
#include "framework/diagnostics/report.h"

namespace Gorc {
namespace Cog {
namespace Grammar {

class Instance {
private:
	Text::Source& inputFilestream;

	// Scanner initialization members
	// implemented in gra_lexer.lex
	void InitScanner();
	void DestroyScanner();

	void* scanner;
	AST::TranslationUnit* ReturnValue;
	const char* CurrentFilename;

public:
	AST::Factory& Factory;
	Diagnostics::Report& Report;

	Instance(Text::Source& inputFilestream, AST::Factory& ast, Diagnostics::Report& report);
	~Instance();

	inline char GetNext() {
		return inputFilestream.GetNext();
	}

	AST::TranslationUnit* Parse();

	inline void* GetScanner() const {
		return scanner;
	}

	inline void SetReturnValue(AST::TranslationUnit* astroot) {
		ReturnValue = astroot;
	}

	inline const char* GetFilename() const {
		return CurrentFilename;
	}

	std::string TokenBuffer;
};

}
}
}
