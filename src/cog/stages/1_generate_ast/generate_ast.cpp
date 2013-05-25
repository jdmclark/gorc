#include "cog/stages/stages.h"
#include "cog/ast/node.h"
#include "cog/ast/factory.h"
#include "framework/diagnostics/report.h"
#include "framework/text/source.h"

#include "cog/grammar/instance.h"

Gorc::Cog::AST::TranslationUnit* Gorc::Cog::Stages::GenerateAST::GenerateAST(Text::Source& inputFilestream,
		Diagnostics::Report& report, AST::Factory& factory) {
	Grammar::Instance instance(inputFilestream, factory, report);
	return instance.Parse();
}
