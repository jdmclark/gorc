#include "virtual_machine.hpp"
#include "opcode.hpp"
#include "continuation.hpp"
#include "restart_exception.hpp"
#include "suspend_exception.hpp"
#include "executor.hpp"
#include "instance.hpp"
#include "io/binary_input_stream.hpp"

gorc::cog::value gorc::cog::virtual_machine::internal_execute(verb_table &verbs,
                                                              executor &exec,
                                                              service_registry &services,
                                                              continuation &cc)
{
    if(cc.call_stack.empty()) {
        // Cannot execute in empty continuation
        return value();
    }

    // Add current continuation to services.
    // System verbs must modify the current continuation.
    services.add_or_replace(cc);

    instance *current_instance = &exec.get_instance(cc.frame().instance_id);
    memory_file::reader sr(current_instance->cog->program);
    sr.set_position(cc.frame().program_counter);

    binary_input_stream bsr(sr);

    while(true) {
        opcode op = binary_deserialize<opcode>(bsr);
        switch(op) {
        case opcode::push: {
                cog::value v(deserialization_constructor, bsr);
                cc.data_stack.push_back(v);
            }
            break;

        case opcode::dup: {
                cog::value v(cc.data_stack.back());
                cc.data_stack.push_back(v);
            }
            break;

        case opcode::load: {
                size_t addr = binary_deserialize<size_t>(bsr);
                cc.data_stack.push_back(current_instance->memory[addr]);
            }
            break;

        case opcode::loadi: {
                int addr = static_cast<int>(binary_deserialize<size_t>(bsr));
                int idx = static_cast<int>(cc.data_stack.back());
                cc.data_stack.pop_back();

                cc.data_stack.push_back(current_instance->memory[static_cast<size_t>(addr + idx)]);
            }
            break;

        case opcode::stor: {
                size_t addr = binary_deserialize<size_t>(bsr);
                current_instance->memory[addr] = cc.data_stack.back();
                cc.data_stack.pop_back();
            }
            break;

        case opcode::stori: {
                int addr = static_cast<int>(binary_deserialize<size_t>(bsr));
                int idx = static_cast<int>(cc.data_stack.back());
                cc.data_stack.pop_back();

                current_instance->memory[static_cast<size_t>(addr + idx)] = cc.data_stack.back();
                cc.data_stack.pop_back();
            }
            break;

        case opcode::jmp: {
                size_t addr = binary_deserialize<size_t>(bsr);
                sr.set_position(addr);
            }
            break;

        case opcode::jal: {
                size_t addr = binary_deserialize<size_t>(bsr);

                // Store current offset in current continuation
                cc.call_stack.back().program_counter = sr.position();

                // Create new stack frame
                cc.call_stack.push_back(call_stack_frame(cc.frame().instance_id,
                                                         addr,
                                                         cc.frame().sender,
                                                         cc.frame().sender_id,
                                                         cc.frame().source,
                                                         cc.frame().param0,
                                                         cc.frame().param1,
                                                         cc.frame().param2,
                                                         cc.frame().param3));

                // Jump
                sr.set_position(addr);
            }
            break;

        case opcode::bt: {
                size_t addr = binary_deserialize<size_t>(bsr);
                cog::value v(cc.data_stack.back());
                cc.data_stack.pop_back();

                if(static_cast<bool>(v)) {
                    sr.set_position(addr);
                }
            }
            break;

        case opcode::bf: {
                size_t addr = binary_deserialize<size_t>(bsr);
                cog::value v(cc.data_stack.back());
                cc.data_stack.pop_back();

                if(!static_cast<bool>(v)) {
                    sr.set_position(addr);
                }
            }
            break;

        case opcode::call: {
                int vid = binary_deserialize<int>(bsr);

                // Store current offset in current continuation
                cc.call_stack.back().program_counter = sr.position();

                verbs.get_verb(verb_id(vid)).invoke(cc.data_stack,
                                                    services,
                                                    /* expects value */ false);
            }
            break;

        case opcode::callv: {
                int vid = binary_deserialize<int>(bsr);

                // Store current offset in current continuation
                cc.call_stack.back().program_counter = sr.position();

                cog::value rv = verbs.get_verb(verb_id(vid)).invoke(cc.data_stack,
                                                                    services,
                                                                    /* expects value */ true);
                cc.data_stack.push_back(rv);
            }
            break;

        case opcode::ret: {
                // Retire top stack frame.
                value return_register = cc.frame().return_register;
                bool save_return_register = cc.frame().save_return_register;
                bool push_return_register = cc.frame().push_return_register;

                cc.call_stack.pop_back();

                if(cc.call_stack.empty()) {
                    return return_register;
                }

                current_instance = &exec.get_instance(cc.call_stack.back().instance_id);
                sr = memory_file::reader(current_instance->cog->program);
                sr.set_position(cc.call_stack.back().program_counter);

                if(save_return_register) {
                    cc.frame().return_register = return_register;
                }

                if(push_return_register) {
                    cc.data_stack.push_back(return_register);
                }
            }
            break;

        case opcode::neg: {
                cog::value v = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(-v);
            }
            break;

        case opcode::lnot: {
                cog::value v = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(!v);
            }
            break;

        case opcode::add: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x + y);
            }
            break;

        case opcode::sub: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x - y);
            }
            break;

        case opcode::mul: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x * y);
            }
            break;

        case opcode::div: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x / y);
            }
            break;

        case opcode::mod: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x % y);
            }
            break;

        case opcode::bor: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x | y);
            }
            break;

        case opcode::band: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x & y);
            }
            break;

        case opcode::bxor: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x ^ y);
            }
            break;

        case opcode::lor: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x || y);
            }
            break;

        case opcode::land: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x && y);
            }
            break;

        case opcode::eq: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x == y);
            }
            break;

        case opcode::ne: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x != y);
            }
            break;

        case opcode::gt: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x > y);
            }
            break;

        case opcode::ge: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x >= y);
            }
            break;

        case opcode::lt: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x < y);
            }
            break;

        case opcode::le: {
                cog::value y = cc.data_stack.back();
                cc.data_stack.pop_back();
                cog::value x = cc.data_stack.back();
                cc.data_stack.pop_back();
                cc.data_stack.push_back(x <= y);
            }
            break;
        }
    }
}

gorc::cog::value gorc::cog::virtual_machine::execute(verb_table &verbs,
                                                     executor &exec,
                                                     service_registry &services,
                                                     continuation &cc)
{
    while(true) {
        try {
            return internal_execute(verbs, exec, services, cc);
        }
        catch(restart_exception const &) {
            // Some engine component has changed the current continuation and
            // is requesting immediate out-of-band execution restart.
            continue;
        }
        catch(suspend_exception const &) {
            // Some engine component has requested immediate out-of-band execution
            // suspension. The current context is stored in the continuation.
            break;
        }

        break;
    }

    // Execution has halted out-of-band. Use whatever value is in the current
    // return register.
    return cc.frame().return_register;
}
