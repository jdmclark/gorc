#pragma once

#include "printer.h"
#include "cog/symbols/table.h"
#include "cog/verbs/table.h"
#include "cog/vm/code_buffer.h"
#include "cog/vm/code_buffer_write_stream.h"
#include "cog/vm/jump_table.h"
#include "cog/message_id.h"

#include <iostream>
#include <unordered_map>

namespace gorc {
namespace cog {
namespace ir {

class code_printer : public printer {
private:
    vm::code_buffer& codeBuffer;
    vm::code_buffer_write_stream stream;
    const symbols::symbol_table& symbol_table;
    const verbs::verb_table& verb_table;
    const std::unordered_map<std::string, message_id>& MessageTable;
    vm::jump_table& JumpTable;

    std::unordered_map<std::string, size_t> labelmap;
    std::unordered_multimap<std::string, size_t> backpatchmap;

public:
    code_printer(vm::code_buffer& codeBuffer, const symbols::symbol_table& symbolTable,
        const std::unordered_map<std::string, message_id>& messageTable, const verbs::verb_table& verbTable,
        vm::jump_table& jumpTable);

    void backpatch();

    void comment(const std::string& msg);
    void label(const std::string& name);

    void nop();

    void copy();
    void constant(const vm::value& value);

    void load(const std::string& symbol);
    void loadi(const std::string& symbol);
    void store(const std::string& symbol);
    void storei(const std::string& symbol);

    void jmp(const std::string& label);
    void jal(const std::string& label);
    void bt(const std::string& label);
    void bf(const std::string& label);
    void call(const std::string& verb);
    void callv(const std::string& verb);
    void ret();

    void neg();
    void add();
    void sub();
    void mul();
    void div();
    void mod();
    void band();
    void bor();
    void bxor();

    void lnot();
    void land();
    void lor();

    void cgt();
    void cgeq();
    void clt();
    void cleq();
    void ceq();
    void cneq();
};

}
}
}
