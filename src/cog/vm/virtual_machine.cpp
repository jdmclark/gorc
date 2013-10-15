#include "virtual_machine.h"
#include "code_buffer_read_stream.h"
#include "opcode.h"

void gorc::cog::vm::virtual_machine::execute(std::vector<value>& heap, const code_buffer& code, size_t pc, const verbs::verb_table& verbTable) {
	allow_run = true;

	code_buffer_read_stream stream(code);
	stream.seek(pc);

	while(allow_run) {
		opcode op = stream.read<opcode>();

		switch(op) {
		case opcode::NOP:
			break;

		case opcode::COPY: {
				value v = stack.top();
				push(v);
			}
			break;

		case opcode::CONST: {
				value v = stream.read<value>();
				push(v);
			}
			break;

		case opcode::LOAD: {
				size_t addr = stream.read<size_t>();
				push(heap[addr]);
			}
			break;

		case opcode::LOADI: {
				size_t addr = stream.read<size_t>();
				int offset = static_cast<int>(pop());
				push(heap[addr + offset]);
			}
			break;

		case opcode::STORE: {
				size_t addr = stream.read<size_t>();
				heap[addr] = pop();
			}
			break;

		case opcode::STOREI: {
				size_t addr = stream.read<size_t>();
				int offset = static_cast<int>(pop());
				heap[addr + offset] = pop();
			}
			break;

		case opcode::JMP: {
				size_t addr = stream.read<size_t>();
				stream.seek(addr);
			}
			break;

		case opcode::JAL: {
				size_t addr = stream.read<size_t>();
				execute(heap, code, addr, verbTable);
			}
			break;

		case opcode::BT: {
				size_t addr = stream.read<size_t>();
				bool condition = static_cast<bool>(pop());
				if(condition) {
					stream.seek(addr);
				}
			}
			break;

		case opcode::BF: {
				size_t addr = stream.read<size_t>();
				bool condition = static_cast<bool>(pop());
				if(!condition) {
					stream.seek(addr);
				}
			}
			break;

		case opcode::CALL: {
				verbs::verb_id verb = stream.read<verbs::verb_id>();
				program_counter = stream.tell(); // Stash program counter for engine code.
				verbTable.invoke(verb, stack);
			}
			break;

		case opcode::CALLV: {
				verbs::verb_id verb = stream.read<verbs::verb_id>();
				push(verbTable.invoke(verb, stack));
			}
			break;

		case opcode::RET:
			return;

		case opcode::NEG: {
				value v = pop();
				push(-v);
			}
			break;

		case opcode::ADD: {
				value right = pop();
				value left = pop();
				push(left + right);
			}
			break;

		case opcode::SUB: {
				value right = pop();
				value left = pop();
				push(left - right);
			}
			break;

		case opcode::MUL: {
				value right = pop();
				value left = pop();
				push(left * right);
			}
			break;

		case opcode::DIV: {
				value right = pop();
				value left = pop();
				push(left / right);
			}
			break;

		case opcode::MOD: {
				value right = pop();
				value left = pop();
				push(left % right);
			}
			break;

		case opcode::AND: {
				value right = pop();
				value left = pop();
				push(left & right);
			}
			break;

		case opcode::OR: {
				value right = pop();
				value left = pop();
				push(left | right);
			}
			break;

		case opcode::XOR: {
				value right = pop();
				value left = pop();
				push(left ^ right);
			}
			break;

		case opcode::LNOT: {
				value right = pop();
				push(!right);
			}
			break;

		case opcode::LAND: {
				value right = pop();
				value left = pop();
				push(left && right);
			}
			break;

		case opcode::LOR: {
				value right = pop();
				value left = pop();
				push(left || right);
			}
			break;

		case opcode::CGT: {
				value right = pop();
				value left = pop();
				push(left > right);
			}
			break;

		case opcode::CGEQ: {
				value right = pop();
				value left = pop();
				push(left >= right);
			}
			break;

		case opcode::CLT: {
				value right = pop();
				value left = pop();
				push(left < right);
			}
			break;

		case opcode::CLEQ: {
				value right = pop();
				value left = pop();
				push(left <= right);
			}
			break;

		case opcode::CEQ: {
				value right = pop();
				value left = pop();
				push(left == right);
			}
			break;

		case opcode::CNEQ: {
				value right = pop();
				value left = pop();
				push(left != right);
			}
			break;
		}
	}
}
