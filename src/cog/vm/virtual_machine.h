#pragma once

#include "value.h"
#include "code_buffer.h"
#include "cog/verbs/table.h"
#include <vector>
#include <stack>

namespace Gorc {
namespace Cog {
namespace VM {

class VirtualMachine {
private:
	std::stack<Value> stack;
	bool allow_run;
	unsigned int program_counter;

public:
	void Execute(std::vector<Value>& heap, const CodeBuffer& code, size_t pc, const Verbs::VerbTable& verbTable);

	inline void Abort() {
		allow_run = false;
	}

	inline void Push(const Value& val) {
		stack.push(val);
	}

	inline Value Pop() {
		Value val = stack.top();
		stack.pop();
		return val;
	}

	inline unsigned int GetProgramCounter() const {
		return program_counter;
	}
};

}
}
}
