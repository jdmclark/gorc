#include "codegen_test_fixture.h"
#include "cog/stages/stages.h"
#include "framework/io/nativefile.h"
#include "cog/vm/virtualmachine.h"
#include "cog/vm/exception.h"
#include "cog/ir/codeprinter.h"
#include <fstream>

using namespace Gorc::Cog;

CodegenTestFixture::CodegenTestFixture(const boost::filesystem::path& BasePath)
	: LanguageTestFixture(BasePath) {
	PopulateTables();
}

CodegenTestFixture::CodegenTestFixture(const Gorc::Content::FileSystem& fs)
	: LanguageTestFixture(fs) {
	PopulateTables();
}

void CodegenTestFixture::PopulateTables() {
#define MSG(x, y) MessageTable.insert(std::make_pair(x, y))
		MSG("activate", MessageId::Activated);
		MSG("activated", MessageId::Activated);
		MSG("aievent", MessageId::AiEvent);
		MSG("arrived", MessageId::Arrived);
		MSG("autoselect", MessageId::Autoselect);
		MSG("blocked", MessageId::Blocked);
		MSG("changed", MessageId::Changed);
		MSG("created", MessageId::Created);
		MSG("crossed", MessageId::Crossed);
		MSG("damaged", MessageId::Damaged);
		MSG("deactivated", MessageId::Deactivated);
		MSG("deselected", MessageId::Deselected);
		MSG("entered", MessageId::Entered);
		MSG("exited", MessageId::Exited);
		MSG("fire", MessageId::Fire);
		MSG("global0", MessageId::Global0);
		MSG("join", MessageId::Join);
		MSG("killed", MessageId::Killed);
		MSG("leave", MessageId::Leave);
		MSG("loading", MessageId::Loading);
		MSG("newplayer", MessageId::NewPlayer);
		MSG("pulse", MessageId::Pulse);
		MSG("removed", MessageId::Removed);
		MSG("respawn", MessageId::Respawn);
		MSG("selected", MessageId::Selected);
		MSG("shutdown", MessageId::Shutdown);
		MSG("sighted", MessageId::Sighted);
		MSG("skill", MessageId::Skill);
		MSG("splash", MessageId::Splash);
		MSG("startup", MessageId::Startup);
		MSG("taken", MessageId::Taken);
		MSG("timer", MessageId::Timer);
		MSG("touched", MessageId::Touched);
		MSG("trigger", MessageId::Trigger);
		MSG("user0", MessageId::User0);
		MSG("user1", MessageId::User1);
		MSG("user2", MessageId::User2);
		MSG("user3", MessageId::User3);
		MSG("user4", MessageId::User4);
		MSG("user5", MessageId::User5);
		MSG("user6", MessageId::User6);
		MSG("user7", MessageId::User7);
	#undef MSG

	return;
}

void CodegenTestFixture::ParseFile(const boost::filesystem::path& filename) {
	Gorc::Cog::AST::Factory astFactory;

	std::unique_ptr<Gorc::IO::ReadOnlyFile> file;
	try {
		file = FileSystem.Open(filename);
	}
	catch(const std::exception& e) {
		Report.AddCriticalError("CodegenTestFixture", "could not open file");
		return;
	}

	Gorc::Text::Source source(*file);

	Gorc::Cog::AST::TranslationUnit* ast = Gorc::Cog::Stages::GenerateAST::GenerateAST(source, Report, astFactory);

	if(Report.GetErrorCount() != 0) {
		return;
	}

	Gorc::Cog::Stages::SemanticAnalysis::SemanticAnalysis(ast, Script.SymbolTable, ConstantTable, VerbTable, Report);

	if(Report.GetErrorCount() != 0) {
		return;
	}

	Gorc::Cog::Stages::ConstantFolding::ConstantFolding(astFactory, ast, Script.SymbolTable, ConstantTable, Report);

	if(Report.GetErrorCount() != 0) {
		return;
	}

	IR::CodePrinter printer(Script.Code, Script.SymbolTable, MessageTable, VerbTable, Script.JumpTable);

	Gorc::Cog::Stages::GenerateCode::GenerateCode(ast, printer, Report);

	if(Report.GetErrorCount() != 0) {
		return;
	}

	// Build instance
	auto inst = Script.CreateInstance();

	// Execute from the 'startup' message.
	size_t startupAddr = Script.JumpTable.GetTarget(MessageId::Startup);

	Gorc::Cog::VM::VirtualMachine vm;

	try {
		vm.Execute(inst->Heap, inst->Script.Code, startupAddr, VerbTable);
	}
	catch(const Gorc::Cog::VM::CodeBufferOverflowException& e) {
		Report.AddCriticalError("CodeGenTestFixture::ParseFile", "code buffer overflow");
	}
}
