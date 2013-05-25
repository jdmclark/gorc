#include "codeprinter.h"
#include "cog/vm/opcode.h"

using Gorc::Cog::IR::CodePrinter;

Gorc::Cog::IR::CodePrinter::CodePrinter(VM::CodeBuffer& codeBuffer,
	const Symbols::SymbolTable& symbolTable,
	const std::unordered_map<std::string, MessageId>& messageTable,
	const Verbs::VerbTable& verbTable, VM::JumpTable& jumpTable)
	: codeBuffer(codeBuffer), stream(codeBuffer), SymbolTable(symbolTable), VerbTable(verbTable),
	MessageTable(messageTable), JumpTable(jumpTable) {
	return;
}

void CodePrinter::Backpatch() {
	for(const std::pair<std::string, size_t>& label : labelmap) {
		auto jt = MessageTable.find(label.first);
		if(jt != MessageTable.end()) {
			MessageId id = static_cast<MessageId>(static_cast<int>(jt->second));
			JumpTable.SetTarget(id, label.second);
		}
	}

	for(const std::pair<std::string, size_t>& backpatch : backpatchmap) {
		auto jt = labelmap.find(backpatch.first);
		if(jt != labelmap.end()) {
			codeBuffer.Write<size_t>(jt->second, backpatch.second);
		}
		else {
			// Missing labels already reported by semantic analysis.
			// Default to zero.
			codeBuffer.Write<size_t>(0, backpatch.second);
		}
	}
}

void CodePrinter::Comment(const std::string&) {
	return;
}

void CodePrinter::Label(const std::string& name) {
	labelmap.insert(std::make_pair(name, stream.Tell()));
}

void CodePrinter::Nop() {
	stream.Write(VM::Opcode::NOP);
}

void CodePrinter::Copy() {
	stream.Write(VM::Opcode::COPY);
}

void CodePrinter::Const(const VM::Value& value) {
	stream.Write(VM::Opcode::CONST);
	stream.Write(value);
}

void CodePrinter::Load(const std::string& symbol) {
	stream.Write(VM::Opcode::LOAD);
	stream.Write(SymbolTable.GetSymbolIndex(symbol));
}

void CodePrinter::LoadI(const std::string& symbol) {
	stream.Write(VM::Opcode::LOADI);
	stream.Write(SymbolTable.GetSymbolIndex(symbol));
}

void CodePrinter::Store(const std::string& symbol) {
	stream.Write(VM::Opcode::STORE);
	stream.Write(SymbolTable.GetSymbolIndex(symbol));
}

void CodePrinter::StoreI(const std::string& symbol) {
	stream.Write(VM::Opcode::STOREI);
	stream.Write(SymbolTable.GetSymbolIndex(symbol));
}

void CodePrinter::Jmp(const std::string& label) {
	stream.Write(VM::Opcode::JMP);
	backpatchmap.insert(std::make_pair(label, stream.Tell()));
	stream.Write<size_t>(0);
}

void CodePrinter::Jal(const std::string& label) {
	stream.Write(VM::Opcode::JAL);
	backpatchmap.insert(std::make_pair(label, stream.Tell()));
	stream.Write<size_t>(0);
}

void CodePrinter::Bt(const std::string& label) {
	stream.Write(VM::Opcode::BT);
	backpatchmap.insert(std::make_pair(label, stream.Tell()));
	stream.Write<size_t>(0);
}

void CodePrinter::Bf(const std::string& label) {
	stream.Write(VM::Opcode::BF);
	backpatchmap.insert(std::make_pair(label, stream.Tell()));
	stream.Write<size_t>(0);
}

void CodePrinter::Call(const std::string& verb) {
	stream.Write(VM::Opcode::CALL);
	stream.Write(VerbTable.GetVerb(verb));
}

void CodePrinter::CallV(const std::string& verb) {
	stream.Write(VM::Opcode::CALLV);
	stream.Write(VerbTable.GetVerb(verb));
}

void CodePrinter::Ret() {
	stream.Write(VM::Opcode::RET);
}

void CodePrinter::Neg() {
	stream.Write(VM::Opcode::NEG);
}

void CodePrinter::Add() {
	stream.Write(VM::Opcode::ADD);
}

void CodePrinter::Sub() {
	stream.Write(VM::Opcode::SUB);
}

void CodePrinter::Mul() {
	stream.Write(VM::Opcode::MUL);
}

void CodePrinter::Div() {
	stream.Write(VM::Opcode::DIV);
}

void CodePrinter::Mod() {
	stream.Write(VM::Opcode::MOD);
}

void CodePrinter::And() {
	stream.Write(VM::Opcode::AND);
}

void CodePrinter::Or() {
	stream.Write(VM::Opcode::OR);
}

void CodePrinter::Xor() {
	stream.Write(VM::Opcode::XOR);
}

void CodePrinter::LNot() {
	stream.Write(VM::Opcode::LNOT);
}

void CodePrinter::LAnd() {
	stream.Write(VM::Opcode::LAND);
}

void CodePrinter::LOr() {
	stream.Write(VM::Opcode::LOR);
}

void CodePrinter::CGt() {
	stream.Write(VM::Opcode::CGT);
}

void CodePrinter::CGeq() {
	stream.Write(VM::Opcode::CGEQ);
}

void CodePrinter::CLt() {
	stream.Write(VM::Opcode::CLT);
}

void CodePrinter::CLeq() {
	stream.Write(VM::Opcode::CLEQ);
}

void CodePrinter::CEq() {
	stream.Write(VM::Opcode::CEQ);
}

void CodePrinter::CNeq() {
	stream.Write(VM::Opcode::CNEQ);
}
