#pragma once

#include "value.h"
#include "code_buffer.h"
#include "cog/verbs/table.h"
#include <vector>
#include <stack>

namespace gorc {
namespace cog {
namespace vm {

class virtual_machine {
private:
	std::stack<value> stack;
	bool allow_run;
	unsigned int program_counter;

public:
	void execute(std::vector<value>& heap, const code_buffer& code, size_t pc, const verbs::verb_table& verbTable, void* system_ptr);

	inline void abort() {
		allow_run = false;
	}

	inline void push(const value& val) {
		stack.push(val);
	}

	inline value pop() {
		value val = stack.top();
		stack.pop();
		return val;
	}

	inline unsigned int get_program_counter() const {
		return program_counter;
	}
};

}
}
}
