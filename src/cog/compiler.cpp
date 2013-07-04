#include "compiler.h"
#include "framework/text/source.h"
#include "cog/stages/stages.h"
#include "cog/ast/factory.h"
#include "cog/ir/codeprinter.h"
#include "framework/io/exception.h"

void Gorc::Cog::Compiler::AddMessageId(const std::string& name, MessageId value) {
	MessageTable.insert(std::make_pair(name, value));
	ConstantTable.insert(std::make_pair(name, static_cast<int>(value)));
}

void Gorc::Cog::Compiler::PopulateTables() {
	AddMessageId("activate", MessageId::Activated);
	AddMessageId("activated", MessageId::Activated);
	AddMessageId("aievent", MessageId::AiEvent);
	AddMessageId("arrived", MessageId::Arrived);
	AddMessageId("autoselect", MessageId::Autoselect);
	AddMessageId("blocked", MessageId::Blocked);
	AddMessageId("changed", MessageId::Changed);
	AddMessageId("created", MessageId::Created);
	AddMessageId("crossed", MessageId::Crossed);
	AddMessageId("damaged", MessageId::Damaged);
	AddMessageId("deactivated", MessageId::Deactivated);
	AddMessageId("deselected", MessageId::Deselected);
	AddMessageId("entered", MessageId::Entered);
	AddMessageId("exited", MessageId::Exited);
	AddMessageId("fire", MessageId::Fire);
	AddMessageId("global0", MessageId::Global0);
	AddMessageId("join", MessageId::Join);
	AddMessageId("killed", MessageId::Killed);
	AddMessageId("leave", MessageId::Leave);
	AddMessageId("loading", MessageId::Loading);
	AddMessageId("newplayer", MessageId::NewPlayer);
	AddMessageId("pulse", MessageId::Pulse);
	AddMessageId("removed", MessageId::Removed);
	AddMessageId("respawn", MessageId::Respawn);
	AddMessageId("selected", MessageId::Selected);
	AddMessageId("shutdown", MessageId::Shutdown);
	AddMessageId("sighted", MessageId::Sighted);
	AddMessageId("skill", MessageId::Skill);
	AddMessageId("splash", MessageId::Splash);
	AddMessageId("startup", MessageId::Startup);
	AddMessageId("taken", MessageId::Taken);
	AddMessageId("timer", MessageId::Timer);
	AddMessageId("touched", MessageId::Touched);
	AddMessageId("trigger", MessageId::Trigger);
	AddMessageId("user0", MessageId::User0);
	AddMessageId("user1", MessageId::User1);
	AddMessageId("user2", MessageId::User2);
	AddMessageId("user3", MessageId::User3);
	AddMessageId("user4", MessageId::User4);
	AddMessageId("user5", MessageId::User5);
	AddMessageId("user6", MessageId::User6);
	AddMessageId("user7", MessageId::User7);

	return;
}

Gorc::Cog::Compiler::Compiler(Verbs::VerbTable& vt)
	: VerbTable(vt) {
	PopulateTables();
}

void Gorc::Cog::Compiler::Compile(IO::ReadOnlyFile& file, Script& output, Diagnostics::Report& report) const {
	unsigned int prevErrorCount = report.GetErrorCount();

	AST::Factory astFactory;
	Text::Source source(file);

	AST::TranslationUnit* ast = Stages::GenerateAST::GenerateAST(source, report, astFactory);

	if(report.GetErrorCount() != prevErrorCount) {
		throw IO::FileCorruptException();
	}

	Gorc::Cog::Stages::SemanticAnalysis::SemanticAnalysis(ast, output.SymbolTable, ConstantTable, VerbTable, report);

	if(report.GetErrorCount() != prevErrorCount) {
		throw IO::FileCorruptException();
	}

	Gorc::Cog::Stages::ConstantFolding::ConstantFolding(astFactory, ast, output.SymbolTable, ConstantTable, report);

	if(report.GetErrorCount() != prevErrorCount) {
		throw IO::FileCorruptException();
	}

	IR::CodePrinter printer(output.Code, output.SymbolTable, MessageTable, VerbTable, output.JumpTable);

	Gorc::Cog::Stages::GenerateCode::GenerateCode(ast, printer, report);

	if(report.GetErrorCount() != prevErrorCount) {
		throw IO::FileCorruptException();
	}

	output.Flags = FlagSet<CogFlag> { ast->Flags };
}
