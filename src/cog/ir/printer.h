#pragma once

#include "cog/vm/value.h"
#include <string>

namespace gorc {
namespace cog {
namespace ir {

class printer {
public:
    virtual ~printer();

    virtual void backpatch() = 0;

    virtual void comment(const std::string& msg) = 0;
    virtual void label(const std::string& name) = 0;

    virtual void nop() = 0;

    virtual void copy() = 0;
    virtual void constant(const vm::value& value) = 0;

    virtual void load(const std::string& symbol) = 0;
    virtual void loadi(const std::string& symbol) = 0;
    virtual void store(const std::string& symbol) = 0;
    virtual void storei(const std::string& symbol) = 0;

    virtual void jmp(const std::string& label) = 0;
    virtual void jal(const std::string& label) = 0;
    virtual void bt(const std::string& label) = 0;
    virtual void bf(const std::string& label) = 0;
    virtual void call(const std::string& verb) = 0;
    virtual void callv(const std::string& verb) = 0;
    virtual void ret() = 0;

    virtual void neg() = 0;
    virtual void add() = 0;
    virtual void sub() = 0;
    virtual void mul() = 0;
    virtual void div() = 0;
    virtual void mod() = 0;
    virtual void band() = 0;
    virtual void bor() = 0;
    virtual void bxor() = 0;

    virtual void lnot() = 0;
    virtual void land() = 0;
    virtual void lor() = 0;

    virtual void cgt() = 0;
    virtual void cgeq() = 0;
    virtual void clt() = 0;
    virtual void cleq() = 0;
    virtual void ceq() = 0;
    virtual void cneq() = 0;
};

}
}
}
