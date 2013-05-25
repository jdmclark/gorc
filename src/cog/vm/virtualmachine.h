#pragma once

#include "value.h"
#include "codebuffer.h"
#include "cog/verbs/table.h"
#include <vector>
#include <stack>

namespace Gorc {
namespace Cog {
namespace VM {

class VirtualMachine {
private:
	std::stack<Value> stack;

public:
	void Execute(std::vector<Value>& heap, const CodeBuffer& code, size_t pc, const Verbs::VerbTable& verbTable);

	inline void Push(const Value& val) {
		stack.push(val);
	}

	inline Value Pop() {
		Value val = stack.top();
		stack.pop();
		return val;
	}
};

}
}
}
