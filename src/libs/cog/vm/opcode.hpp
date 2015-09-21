#pragma once

#include <cstdint>

namespace gorc {
    namespace cog {

        enum class opcode : uint8_t {
            nop,            // NOP : non-operation

            push,           // PUSH [immediate] : push immediate
            dup,            // DUP : duplicate top value

            load,           // LOAD [address] : loads from heap
            loadi,          // LOADI [address] : load indexed
            stor,           // STOR [address] : stores to heap
            stori,          // STORI [address] : store indexed

            jmp,            // JMP [address] : jump to address
            jal,            // JAL [address] : jump and link
            bt,             // BT [address] : jump if top of stack is true
            bf,             // BF [address] : jump if top of stack is false
            call,           // CALL [verb] : jump and link to registered verb
            callv,          // CALLV [verb] : jump and link to registered verb, returning value

            ret,            // RET : return from link

            neg,            // NEG : negates value on stack
            lnot,           // LNOT : logically negates value on stack

            add,            // ADD : adds values on stack
            sub,            // SUB : subtracts top from bottom
            mul,            // MUL : multiplies on stack
            div,            // DIV : divides top from bottom
            mod,            // MOD : modulo top from bottom

            bor,            // BOR : bitwise or
            band,           // BAND : bitwise and
            bxor,           // BXOR : bitwise xor

            lor,            // LOR : logical or
            land,           // LAND : logical and

            eq,             // EQ : equality
            ne,             // NE : inequality
            gt,             // GT : greater than
            ge,             // GE : greater or equal
            lt,             // LT : less than
            le,             // LE : less or equal
        };

    }
}
