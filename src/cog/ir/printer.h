#pragma once

#include "cog/vm/value.h"
#include <string>

namespace Gorc {
namespace Cog {
namespace IR {

class Printer {
public:
	virtual ~Printer();

	virtual void Backpatch() = 0;

	virtual void Comment(const std::string& msg) = 0;
	virtual void Label(const std::string& name) = 0;

	virtual void Nop() = 0;

	virtual void Copy() = 0;
	virtual void Const(const VM::Value& value) = 0;

	virtual void Load(const std::string& symbol) = 0;
	virtual void LoadI(const std::string& symbol) = 0;
	virtual void Store(const std::string& symbol) = 0;
	virtual void StoreI(const std::string& symbol) = 0;

	virtual void Jmp(const std::string& label) = 0;
	virtual void Jal(const std::string& label) = 0;
	virtual void Bt(const std::string& label) = 0;
	virtual void Bf(const std::string& label) = 0;
	virtual void Call(const std::string& verb) = 0;
	virtual void CallV(const std::string& verb) = 0;
	virtual void Ret() = 0;

	virtual void Neg() = 0;
	virtual void Add() = 0;
	virtual void Sub() = 0;
	virtual void Mul() = 0;
	virtual void Div() = 0;
	virtual void Mod() = 0;
	virtual void And() = 0;
	virtual void Or() = 0;
	virtual void Xor() = 0;

	virtual void LNot() = 0;
	virtual void LAnd() = 0;
	virtual void LOr() = 0;

	virtual void CGt() = 0;
	virtual void CGeq() = 0;
	virtual void CLt() = 0;
	virtual void CLeq() = 0;
	virtual void CEq() = 0;
	virtual void CNeq() = 0;
};

}
}
}
