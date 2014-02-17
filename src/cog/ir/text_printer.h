#pragma once

#include "printer.h"
#include "cog/symbols/table.h"
#include "cog/verbs/table.h"

#include <iostream>

namespace gorc {
namespace cog {
namespace ir {

class textPrinter : public printer {
private:
    std::ostream& stream;

    void writevalue(const vm::value& value);

public:
    textPrinter(std::ostream& os);

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
