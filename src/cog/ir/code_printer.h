#pragma once

#include "printer.h"
#include "cog/symbols/table.h"
#include "cog/verbs/table.h"
#include "cog/vm/code_buffer.h"
#include "cog/vm/code_buffer_write_stream.h"
#include "cog/vm/jump_table.h"
#include "cog/message_id.h"

#include <iostream>
#include <unordered_map>

namespace Gorc {
namespace Cog {
namespace IR {

class CodePrinter : public Printer {
private:
	VM::CodeBuffer& codeBuffer;
	VM::CodeBufferWriteStream stream;
	const Symbols::SymbolTable& SymbolTable;
	const Verbs::VerbTable& VerbTable;
	const std::unordered_map<std::string, MessageId>& MessageTable;
	VM::JumpTable& JumpTable;

	std::unordered_map<std::string, size_t> labelmap;
	std::unordered_multimap<std::string, size_t> backpatchmap;

public:
	CodePrinter(VM::CodeBuffer& codeBuffer, const Symbols::SymbolTable& symbolTable,
		const std::unordered_map<std::string, MessageId>& messageTable, const Verbs::VerbTable& verbTable,
		VM::JumpTable& jumpTable);

	void Backpatch();

	void Comment(const std::string& msg);
	void Label(const std::string& name);

	void Nop();

	void Copy();
	void Const(const VM::Value& value);

	void Load(const std::string& symbol);
	void LoadI(const std::string& symbol);
	void Store(const std::string& symbol);
	void StoreI(const std::string& symbol);

	void Jmp(const std::string& label);
	void Jal(const std::string& label);
	void Bt(const std::string& label);
	void Bf(const std::string& label);
	void Call(const std::string& verb);
	void CallV(const std::string& verb);
	void Ret();

	void Neg();
	void Add();
	void Sub();
	void Mul();
	void Div();
	void Mod();
	void And();
	void Or();
	void Xor();

	void LNot();
	void LAnd();
	void LOr();

	void CGt();
	void CGeq();
	void CLt();
	void CLeq();
	void CEq();
	void CNeq();
};

}
}
}
