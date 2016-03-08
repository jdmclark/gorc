#include "log/log.hpp"
#include "compiler.hpp"
#include "libold/cog/stages/stages.hpp"
#include "libold/cog/ast/factory.hpp"
#include "libold/cog/ir/code_printer.hpp"

void gorc::cog::compiler::add_message_id(const std::string& name, message_id value) {
    MessageTable.insert(std::make_pair(name, value));
    ConstantTable.insert(std::make_pair(name, static_cast<int>(value)));
}

void gorc::cog::compiler::populate_tables() {
    add_message_id("activate", message_id::activated);
    add_message_id("activated", message_id::activated);
    add_message_id("aievent", message_id::ai_event);
    add_message_id("arrived", message_id::arrived);
    add_message_id("autoselect", message_id::autoselect);
    add_message_id("blocked", message_id::blocked);
    add_message_id("changed", message_id::changed);
    add_message_id("created", message_id::created);
    add_message_id("crossed", message_id::crossed);
    add_message_id("damaged", message_id::damaged);
    add_message_id("deactivated", message_id::deactivated);
    add_message_id("deselected", message_id::deselected);
    add_message_id("entered", message_id::entered);
    add_message_id("exited", message_id::exited);
    add_message_id("fire", message_id::fire);
    add_message_id("global0", message_id::global0);
    add_message_id("join", message_id::join);
    add_message_id("killed", message_id::killed);
    add_message_id("leave", message_id::leave);
    add_message_id("loading", message_id::loading);
    add_message_id("newplayer", message_id::new_player);
    add_message_id("pulse", message_id::pulse);
    add_message_id("removed", message_id::removed);
    add_message_id("respawn", message_id::respawn);
    add_message_id("selected", message_id::selected);
    add_message_id("shutdown", message_id::shutdown);
    add_message_id("sighted", message_id::sighted);
    add_message_id("skill", message_id::skill);
    add_message_id("splash", message_id::splash);
    add_message_id("startup", message_id::startup);
    add_message_id("taken", message_id::taken);
    add_message_id("timer", message_id::timer);
    add_message_id("touched", message_id::touched);
    add_message_id("trigger", message_id::trigger);
    add_message_id("user0", message_id::user0);
    add_message_id("user1", message_id::user1);
    add_message_id("user2", message_id::user2);
    add_message_id("user3", message_id::user3);
    add_message_id("user4", message_id::user4);
    add_message_id("user5", message_id::user5);
    add_message_id("user6", message_id::user6);
    add_message_id("user7", message_id::user7);

    return;
}

gorc::cog::compiler::compiler(verbs::verb_table& vt)
    : verb_table(vt) {
    populate_tables();
}

void gorc::cog::compiler::compile(input_stream& file, script& output) const {
    ast::factory astFactory;

    ast::translation_unit* ast = stages::generate_ast::generate_ast(file, astFactory);

    if(diagnostic_file_error_count()) {
        LOG_FATAL("script contains syntax errors");
    }

    gorc::cog::stages::semantic_analysis::semantic_analysis(ast, output.symbol_table, ConstantTable, verb_table);

    if(diagnostic_file_error_count()) {
        LOG_FATAL("script contains semantic errors");
    }

    ir::code_printer printer(output.code, output.symbol_table, MessageTable, verb_table, output.jump_table);

    gorc::cog::stages::generate_code::generate_code(ast, printer, ConstantTable);

    output.flags = flag_set<flags::cog_flag> { ast->flags };
}
