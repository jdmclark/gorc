#include "virtualmachine.h"
#include "codebufferreadstream.h"
#include "opcode.h"

void Gorc::Cog::VM::VirtualMachine::Execute(std::vector<Value>& heap, const CodeBuffer& code, size_t pc, const Verbs::VerbTable& verbTable) {
	allow_run = true;

	CodeBufferReadStream stream(code);
	stream.Seek(pc);

	while(allow_run) {
		Opcode op = stream.Read<Opcode>();

		switch(op) {
		case Opcode::NOP:
			break;

		case Opcode::COPY: {
				Value v = stack.top();
				Push(v);
			}
			break;

		case Opcode::CONST: {
				Value v = stream.Read<Value>();
				Push(v);
			}
			break;

		case Opcode::LOAD: {
				size_t addr = stream.Read<size_t>();
				Push(heap[addr]);
			}
			break;

		case Opcode::LOADI: {
				size_t addr = stream.Read<size_t>();
				int offset = static_cast<int>(Pop());
				Push(heap[addr + offset]);
			}
			break;

		case Opcode::STORE: {
				size_t addr = stream.Read<size_t>();
				heap[addr] = Pop();
			}
			break;

		case Opcode::STOREI: {
				size_t addr = stream.Read<size_t>();
				int offset = static_cast<int>(Pop());
				heap[addr + offset] = Pop();
			}
			break;

		case Opcode::JMP: {
				size_t addr = stream.Read<size_t>();
				stream.Seek(addr);
			}
			break;

		case Opcode::JAL: {
				size_t addr = stream.Read<size_t>();
				Execute(heap, code, addr, verbTable);
			}
			break;

		case Opcode::BT: {
				size_t addr = stream.Read<size_t>();
				bool condition = static_cast<bool>(Pop());
				if(condition) {
					stream.Seek(addr);
				}
			}
			break;

		case Opcode::BF: {
				size_t addr = stream.Read<size_t>();
				bool condition = static_cast<bool>(Pop());
				if(!condition) {
					stream.Seek(addr);
				}
			}
			break;

		case Opcode::CALL: {
				Verbs::VerbId verb = stream.Read<Verbs::VerbId>();
				program_counter = stream.Tell(); // Stash program counter for engine code.
				verbTable.Invoke(verb, stack);
			}
			break;

		case Opcode::CALLV: {
				Verbs::VerbId verb = stream.Read<Verbs::VerbId>();
				Push(verbTable.Invoke(verb, stack));
			}
			break;

		case Opcode::RET:
			return;

		case Opcode::NEG: {
				Value v = Pop();
				Push(-v);
			}
			break;

		case Opcode::ADD: {
				Value right = Pop();
				Value left = Pop();
				Push(left + right);
			}
			break;

		case Opcode::SUB: {
				Value right = Pop();
				Value left = Pop();
				Push(left - right);
			}
			break;

		case Opcode::MUL: {
				Value right = Pop();
				Value left = Pop();
				Push(left * right);
			}
			break;

		case Opcode::DIV: {
				Value right = Pop();
				Value left = Pop();
				Push(left / right);
			}
			break;

		case Opcode::MOD: {
				Value right = Pop();
				Value left = Pop();
				Push(left % right);
			}
			break;

		case Opcode::AND: {
				Value right = Pop();
				Value left = Pop();
				Push(left & right);
			}
			break;

		case Opcode::OR: {
				Value right = Pop();
				Value left = Pop();
				Push(left | right);
			}
			break;

		case Opcode::XOR: {
				Value right = Pop();
				Value left = Pop();
				Push(left ^ right);
			}
			break;

		case Opcode::LNOT: {
				Value right = Pop();
				Push(!right);
			}
			break;

		case Opcode::LAND: {
				Value right = Pop();
				Value left = Pop();
				Push(left && right);
			}
			break;

		case Opcode::LOR: {
				Value right = Pop();
				Value left = Pop();
				Push(left || right);
			}
			break;

		case Opcode::CGT: {
				Value right = Pop();
				Value left = Pop();
				Push(left > right);
			}
			break;

		case Opcode::CGEQ: {
				Value right = Pop();
				Value left = Pop();
				Push(left >= right);
			}
			break;

		case Opcode::CLT: {
				Value right = Pop();
				Value left = Pop();
				Push(left < right);
			}
			break;

		case Opcode::CLEQ: {
				Value right = Pop();
				Value left = Pop();
				Push(left <= right);
			}
			break;

		case Opcode::CEQ: {
				Value right = Pop();
				Value left = Pop();
				Push(left == right);
			}
			break;

		case Opcode::CNEQ: {
				Value right = Pop();
				Value left = Pop();
				Push(left != right);
			}
			break;
		}
	}
}
