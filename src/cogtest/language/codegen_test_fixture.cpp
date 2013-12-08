#include "codegen_test_fixture.h"
#include "cog/stages/stages.h"
#include "framework/io/native_file.h"
#include "cog/vm/virtual_machine.h"
#include "cog/vm/exception.h"
#include "cog/ir/code_printer.h"
#include "cog/instance.h"
#include <fstream>

using namespace gorc::cog;

CodegenTestFixture::CodegenTestFixture(const boost::filesystem::path& BasePath)
	: LanguageTestFixture(BasePath) {
	populate_tables();
}

CodegenTestFixture::CodegenTestFixture(const gorc::content::filesystem& fs)
	: LanguageTestFixture(fs) {
	populate_tables();
}

void CodegenTestFixture::populate_tables() {
#define MSG(x, y) MessageTable.insert(std::make_pair(x, y))
		MSG("activate", message_id::activated);
		MSG("activated", message_id::activated);
		MSG("aievent", message_id::ai_event);
		MSG("arrived", message_id::arrived);
		MSG("autoselect", message_id::autoselect);
		MSG("blocked", message_id::blocked);
		MSG("changed", message_id::changed);
		MSG("created", message_id::created);
		MSG("crossed", message_id::crossed);
		MSG("damaged", message_id::damaged);
		MSG("deactivated", message_id::deactivated);
		MSG("deselected", message_id::deselected);
		MSG("entered", message_id::entered);
		MSG("exited", message_id::exited);
		MSG("fire", message_id::fire);
		MSG("global0", message_id::global0);
		MSG("join", message_id::join);
		MSG("killed", message_id::killed);
		MSG("leave", message_id::leave);
		MSG("loading", message_id::loading);
		MSG("newplayer", message_id::new_player);
		MSG("pulse", message_id::pulse);
		MSG("removed", message_id::removed);
		MSG("respawn", message_id::respawn);
		MSG("selected", message_id::selected);
		MSG("shutdown", message_id::shutdown);
		MSG("sighted", message_id::sighted);
		MSG("skill", message_id::skill);
		MSG("splash", message_id::splash);
		MSG("startup", message_id::startup);
		MSG("taken", message_id::taken);
		MSG("timer", message_id::timer);
		MSG("touched", message_id::touched);
		MSG("trigger", message_id::trigger);
		MSG("user0", message_id::user0);
		MSG("user1", message_id::user1);
		MSG("user2", message_id::user2);
		MSG("user3", message_id::user3);
		MSG("user4", message_id::user4);
		MSG("user5", message_id::user5);
		MSG("user6", message_id::user6);
		MSG("user7", message_id::user7);
	#undef MSG

	return;
}

std::unique_ptr<gorc::cog::instance> CreateInstance(gorc::cog::script& script) {
	std::unique_ptr<gorc::cog::instance> inst(new gorc::cog::instance(script));

	inst->heap.resize(script.symbol_table.size());

	auto it = script.symbol_table.begin();
	auto jt = inst->heap.begin();

	for( ; it != script.symbol_table.end() && jt != inst->heap.end(); ++it, ++jt) {
		(*jt) = it->default_value;
	}

	return std::move(inst);
}

void CodegenTestFixture::ParseFile(const boost::filesystem::path& filename) {
	gorc::cog::ast::factory astFactory;

	std::unique_ptr<gorc::io::read_only_file> file;
	try {
		file = FileSystem.open(filename);
	}
	catch(const std::exception& e) {
		report.add_critical_error("CodegenTestFixture", "could not open file");
		return;
	}

	gorc::text::source source(*file);

	gorc::cog::ast::translation_unit* ast = gorc::cog::stages::generate_ast::generate_ast(source, report, astFactory);

	if(report.get_error_count() != 0) {
		return;
	}

	gorc::cog::stages::semantic_analysis::semantic_analysis(ast, script.symbol_table, ConstantTable, verb_table, report);

	if(report.get_error_count() != 0) {
		return;
	}

	gorc::cog::stages::constant_folding::constant_folding(astFactory, ast, script.symbol_table, ConstantTable, report);

	if(report.get_error_count() != 0) {
		return;
	}

	ir::code_printer printer(script.code, script.symbol_table, MessageTable, verb_table, script.jump_table);

	gorc::cog::stages::generate_code::generate_code(ast, printer, report);

	if(report.get_error_count() != 0) {
		return;
	}

	// Build instance
	auto inst = CreateInstance(script);

	// Execute from the 'startup' message.
	size_t startupAddr = script.jump_table.get_target(message_id::startup);

	gorc::cog::vm::virtual_machine vm;

	try {
		vm.execute(inst->heap, inst->script.code, startupAddr, verb_table, nullptr);
	}
	catch(const gorc::cog::vm::code_buffer_overflow_exception& e) {
		report.add_critical_error("CodeGenTestFixture::ParseFile", "code buffer overflow");
	}
}
