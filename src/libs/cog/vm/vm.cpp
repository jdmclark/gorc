#include "vm.hpp"
#include "opcode.hpp"
#include <stack>

void gorc::cog::vm::execute(verb_table &verbs, script const &script, heap &heap, size_t pc)
{
    std::stack<cog::value> stack;

    memory_file::reader sr(script.program);
    sr.set_position(pc);

    while(true) {
        opcode op = read<opcode>(sr);
        switch(op) {
        case opcode::push: {
                cog::value v(deserialization_constructor, sr);
                stack.push(v);
            }
            break;

        case opcode::dup: {
                cog::value v(stack.top());
                stack.push(v);
            }
            break;

        case opcode::load: {
                size_t addr = read<size_t>(sr);
                stack.push(heap[addr]);
            }
            break;

        case opcode::loadi: {
                size_t addr = read<size_t>(sr);
                int idx = static_cast<int>(stack.top());
                stack.pop();

                stack.push(heap[addr + idx]);
            }
            break;

        case opcode::stor: {
                size_t addr = read<size_t>(sr);
                heap[addr] = stack.top();
                stack.pop();
            }
            break;

        case opcode::stori: {
                size_t addr = read<size_t>(sr);
                int idx = static_cast<int>(stack.top());
                stack.pop();

                heap[addr + idx] = stack.top();
                stack.pop();
            }
            break;

        case opcode::jmp: {
                size_t addr = read<size_t>(sr);
                sr.set_position(addr);
            }
            break;

        case opcode::jal: {
                size_t addr = read<size_t>(sr);

                // Store current offset on the stack
                stack.push(cog::value(static_cast<int>(sr.position())));

                sr.set_position(addr);
            }
            break;

        case opcode::bt: {
                size_t addr = read<size_t>(sr);
                cog::value v(stack.top());
                stack.pop();

                if(static_cast<bool>(v)) {
                    sr.set_position(addr);
                }
            }
            break;

        case opcode::bf: {
                size_t addr = read<size_t>(sr);
                cog::value v(stack.top());
                stack.pop();

                if(!static_cast<bool>(v)) {
                    sr.set_position(addr);
                }
            }
            break;

        case opcode::call: {
                int vid = read<int>(sr);
                verbs.get_verb(verb_id(vid)).invoke(stack);
            }
            break;

        case opcode::callv: {
                int vid = read<int>(sr);
                cog::value rv = verbs.get_verb(verb_id(vid)).invoke(stack);
                stack.push(rv);
            }
            break;

        case opcode::ret: {
                // Check stack for return address
                if(stack.empty()) {
                    return;
                }
                else {
                    int pc = static_cast<int>(stack.top());
                    stack.pop();

                    sr.set_position(pc);
                }
            }
            break;

        case opcode::neg: {
                cog::value v = stack.top();
                stack.pop();
                stack.push(-v);
            }
            break;

        case opcode::lnot: {
                cog::value v = stack.top();
                stack.pop();
                stack.push(!v);
            }
            break;

        case opcode::add: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x + y);
            }
            break;

        case opcode::sub: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x - y);
            }
            break;

        case opcode::mul: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x * y);
            }
            break;

        case opcode::div: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x / y);
            }
            break;

        case opcode::mod: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x % y);
            }
            break;

        case opcode::bor: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x | y);
            }
            break;

        case opcode::band: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x & y);
            }
            break;

        case opcode::bxor: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x ^ y);
            }
            break;

        case opcode::lor: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x || y);
            }
            break;

        case opcode::land: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x && y);
            }
            break;

        case opcode::eq: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x == y);
            }
            break;

        case opcode::ne: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x != y);
            }
            break;

        case opcode::gt: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x > y);
            }
            break;

        case opcode::ge: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x >= y);
            }
            break;

        case opcode::lt: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x < y);
            }
            break;

        case opcode::le: {
                cog::value y = stack.top();
                stack.pop();
                cog::value x = stack.top();
                stack.pop();
                stack.push(x <= y);
            }
            break;
        }
    }
}
