#include "code_printer.hpp"
#include "libold/cog/vm/opcode.hpp"

using gorc::cog::ir::code_printer;

gorc::cog::ir::code_printer::code_printer(vm::code_buffer& codeBuffer,
    const symbols::symbol_table& symbolTable,
    const std::unordered_map<std::string, message_id>& messageTable,
    const verbs::verb_table& verbTable, vm::jump_table& jumpTable)
    : codeBuffer(codeBuffer), stream(codeBuffer), symbol_table(symbolTable), verb_table(verbTable),
    MessageTable(messageTable), JumpTable(jumpTable) {
    return;
}

void code_printer::backpatch() {
    for(const std::pair<std::string, size_t>& label : labelmap) {
        auto jt = MessageTable.find(label.first);
        if(jt != MessageTable.end()) {
            message_id id = static_cast<message_id>(static_cast<int>(jt->second));
            JumpTable.set_target(id, label.second);
        }
    }

    for(const std::pair<std::string, size_t>& backpatch : backpatchmap) {
        auto jt = labelmap.find(backpatch.first);
        if(jt != labelmap.end()) {
            codeBuffer.write<size_t>(jt->second, backpatch.second);
        }
        else {
            // Missing labels already reported by semantic analysis.
            // Default to zero.
            codeBuffer.write<size_t>(0, backpatch.second);
        }
    }
}

void code_printer::comment(const std::string&) {
    return;
}

void code_printer::label(const std::string& name) {
    labelmap.insert(std::make_pair(name, stream.tell()));
}

void code_printer::nop() {
    stream.write(vm::opcode::NOP);
}

void code_printer::copy() {
    stream.write(vm::opcode::COPY);
}

void code_printer::constant(const vm::value& value) {
    stream.write(vm::opcode::CONST);
    stream.write(&value, sizeof(value));
}

void code_printer::load(const std::string& symbol) {
    stream.write(vm::opcode::LOAD);
    stream.write(symbol_table.get_symbol_index(symbol));
}

void code_printer::loadi(const std::string& symbol) {
    stream.write(vm::opcode::LOADI);
    stream.write(symbol_table.get_symbol_index(symbol));
}

void code_printer::store(const std::string& symbol) {
    stream.write(vm::opcode::STORE);
    stream.write(symbol_table.get_symbol_index(symbol));
}

void code_printer::storei(const std::string& symbol) {
    stream.write(vm::opcode::STOREI);
    stream.write(symbol_table.get_symbol_index(symbol));
}

void code_printer::jmp(const std::string& label) {
    stream.write(vm::opcode::JMP);
    backpatchmap.insert(std::make_pair(label, stream.tell()));
    stream.write<size_t>(0);
}

void code_printer::jal(const std::string& label) {
    stream.write(vm::opcode::JAL);
    backpatchmap.insert(std::make_pair(label, stream.tell()));
    stream.write<size_t>(0);
}

void code_printer::bt(const std::string& label) {
    stream.write(vm::opcode::BT);
    backpatchmap.insert(std::make_pair(label, stream.tell()));
    stream.write<size_t>(0);
}

void code_printer::bf(const std::string& label) {
    stream.write(vm::opcode::BF);
    backpatchmap.insert(std::make_pair(label, stream.tell()));
    stream.write<size_t>(0);
}

void code_printer::call(const std::string& verb) {
    stream.write(vm::opcode::CALL);
    stream.write(verb_table.get_verb(verb));
}

void code_printer::callv(const std::string& verb) {
    stream.write(vm::opcode::CALLV);
    stream.write(verb_table.get_verb(verb));
}

void code_printer::ret() {
    stream.write(vm::opcode::RET);
}

void code_printer::neg() {
    stream.write(vm::opcode::NEG);
}

void code_printer::add() {
    stream.write(vm::opcode::ADD);
}

void code_printer::sub() {
    stream.write(vm::opcode::SUB);
}

void code_printer::mul() {
    stream.write(vm::opcode::MUL);
}

void code_printer::div() {
    stream.write(vm::opcode::DIV);
}

void code_printer::mod() {
    stream.write(vm::opcode::MOD);
}

void code_printer::band() {
    stream.write(vm::opcode::AND);
}

void code_printer::bor() {
    stream.write(vm::opcode::OR);
}

void code_printer::bxor() {
    stream.write(vm::opcode::XOR);
}

void code_printer::lnot() {
    stream.write(vm::opcode::LNOT);
}

void code_printer::land() {
    stream.write(vm::opcode::LAND);
}

void code_printer::lor() {
    stream.write(vm::opcode::LOR);
}

void code_printer::cgt() {
    stream.write(vm::opcode::CGT);
}

void code_printer::cgeq() {
    stream.write(vm::opcode::CGEQ);
}

void code_printer::clt() {
    stream.write(vm::opcode::CLT);
}

void code_printer::cleq() {
    stream.write(vm::opcode::CLEQ);
}

void code_printer::ceq() {
    stream.write(vm::opcode::CEQ);
}

void code_printer::cneq() {
    stream.write(vm::opcode::CNEQ);
}
