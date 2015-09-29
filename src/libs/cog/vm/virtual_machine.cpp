#include "virtual_machine.hpp"
#include "opcode.hpp"
#include "continuation.hpp"

void gorc::cog::virtual_machine::execute(verb_table &verbs,
                                         service_registry &services,
                                         continuation &cc)
{
    if(cc.call_stack.empty()) {
        // Cannot execute in empty continuation
        return;
    }

    // Add current continuation to services.
    // System verbs must modify the current continuation.
    services.add_or_replace(cc);

    call_stack_frame *frame = &cc.call_stack.top();

    memory_file::reader sr(frame->cog.program);
    sr.set_position(frame->program_counter);

    while(true) {
        opcode op = read<opcode>(sr);
        switch(op) {
        case opcode::push: {
                cog::value v(deserialization_constructor, sr);
                cc.data_stack.push(v);
            }
            break;

        case opcode::dup: {
                cog::value v(cc.data_stack.top());
                cc.data_stack.push(v);
            }
            break;

        case opcode::load: {
                size_t addr = read<size_t>(sr);
                cc.data_stack.push(frame->memory[addr]);
            }
            break;

        case opcode::loadi: {
                size_t addr = read<size_t>(sr);
                int idx = static_cast<int>(cc.data_stack.top());
                cc.data_stack.pop();

                cc.data_stack.push(frame->memory[addr + idx]);
            }
            break;

        case opcode::stor: {
                size_t addr = read<size_t>(sr);
                frame->memory[addr] = cc.data_stack.top();
                cc.data_stack.pop();
            }
            break;

        case opcode::stori: {
                size_t addr = read<size_t>(sr);
                int idx = static_cast<int>(cc.data_stack.top());
                cc.data_stack.pop();

                frame->memory[addr + idx] = cc.data_stack.top();
                cc.data_stack.pop();
            }
            break;

        case opcode::jmp: {
                size_t addr = read<size_t>(sr);
                sr.set_position(addr);
            }
            break;

        case opcode::jal: {
                size_t addr = read<size_t>(sr);

                // Store current offset in current continuation
                cc.call_stack.top().program_counter = sr.position();

                // Create new stack frame
                cc.call_stack.push(call_stack_frame(frame->cog, frame->memory, addr));

                // Jump
                sr.set_position(addr);
            }
            break;

        case opcode::bt: {
                size_t addr = read<size_t>(sr);
                cog::value v(cc.data_stack.top());
                cc.data_stack.pop();

                if(static_cast<bool>(v)) {
                    sr.set_position(addr);
                }
            }
            break;

        case opcode::bf: {
                size_t addr = read<size_t>(sr);
                cog::value v(cc.data_stack.top());
                cc.data_stack.pop();

                if(!static_cast<bool>(v)) {
                    sr.set_position(addr);
                }
            }
            break;

        case opcode::call: {
                int vid = read<int>(sr);
                verbs.get_verb(verb_id(vid)).invoke(cc.data_stack, services);
            }
            break;

        case opcode::callv: {
                int vid = read<int>(sr);
                cog::value rv = verbs.get_verb(verb_id(vid)).invoke(cc.data_stack, services);
                cc.data_stack.push(rv);
            }
            break;

        case opcode::ret: {
                // Retire top stack frame. TODO: ReturnEx value.
                cc.call_stack.pop();

                if(cc.call_stack.empty()) {
                    return;
                }

                sr = memory_file::reader(cc.call_stack.top().cog.program);
                sr.set_position(cc.call_stack.top().program_counter);
            }
            break;

        case opcode::neg: {
                cog::value v = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(-v);
            }
            break;

        case opcode::lnot: {
                cog::value v = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(!v);
            }
            break;

        case opcode::add: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x + y);
            }
            break;

        case opcode::sub: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x - y);
            }
            break;

        case opcode::mul: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x * y);
            }
            break;

        case opcode::div: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x / y);
            }
            break;

        case opcode::mod: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x % y);
            }
            break;

        case opcode::bor: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x | y);
            }
            break;

        case opcode::band: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x & y);
            }
            break;

        case opcode::bxor: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x ^ y);
            }
            break;

        case opcode::lor: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x || y);
            }
            break;

        case opcode::land: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x && y);
            }
            break;

        case opcode::eq: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x == y);
            }
            break;

        case opcode::ne: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x != y);
            }
            break;

        case opcode::gt: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x > y);
            }
            break;

        case opcode::ge: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x >= y);
            }
            break;

        case opcode::lt: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x < y);
            }
            break;

        case opcode::le: {
                cog::value y = cc.data_stack.top();
                cc.data_stack.pop();
                cog::value x = cc.data_stack.top();
                cc.data_stack.pop();
                cc.data_stack.push(x <= y);
            }
            break;
        }
    }
}
