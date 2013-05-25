#include "instance.h"

Gorc::Cog::Grammar::Instance::Instance(Text::Source& inputFilestream, AST::Factory& ast, Diagnostics::Report& report)
	: inputFilestream(inputFilestream), Factory(ast), Report(report) {
	std::string* stored_filename = ast.StoreValue(inputFilestream.Filename);
	CurrentFilename = stored_filename->c_str();

	InitScanner();
	return;
}

Gorc::Cog::Grammar::Instance::~Instance() {
	DestroyScanner();
}

int gra_parse(Gorc::Cog::Grammar::Instance*);

Gorc::Cog::AST::TranslationUnit* Gorc::Cog::Grammar::Instance::Parse() {
	gra_parse(this);
	return ReturnValue;
}
