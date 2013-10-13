#pragma once

#include "printer.h"
#include "cog/symbols/table.h"
#include "cog/verbs/table.h"

#include <iostream>

namespace Gorc {
namespace Cog {
namespace IR {

class TextPrinter : public Printer {
private:
	std::ostream& stream;

	void writeValue(const VM::Value& value);

public:
	TextPrinter(std::ostream& os);

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
