#include "disassembler.hpp"
#include "cog/vm/opcode.hpp"
#include "io/binary_input_stream.hpp"
#include <map>
#include <sstream>
#include <iostream>

void gorc::disassemble_code(cog::script &s, cog::verb_table &verbs)
{
    std::cout << "DISASSEMBLY" << std::endl << std::endl;;

    std::map<size_t, std::tuple<std::string, maybe<size_t>>> lines;
    std::map<size_t, std::string> labels;

    // Pre-populate labels with exported message names
    for(auto const &msg : s.exports) {
        labels.emplace(msg.second, as_string(msg.first));
    }

    auto lazy_add_default_addr = [&](size_t addr)
    {
        auto it = labels.find(addr);
        if(it == labels.end()) {
            labels.emplace(addr, str(format("L%d") % addr));
        }

        return addr;
    };

    memory_file::reader mfr(s.program);
    binary_input_stream r(mfr);
    while(!r.at_end()) {
        size_t line_addr = mfr.position();
        maybe<size_t> printed_address = nothing;
        std::stringstream line;

        auto op = binary_deserialize<cog::opcode>(r);
        switch(op) {
        case cog::opcode::push:
            line << "push ";
            line << cog::value(deserialization_constructor, r).as_string();
            break;
        case cog::opcode::dup:
            line << "dup";
            break;
        case cog::opcode::load:
            line << "load ";
            line << binary_deserialize<size_t>(r);
            break;
        case cog::opcode::loadi:
            line << "loadi ";
            line << binary_deserialize<size_t>(r);
            break;
        case cog::opcode::stor:
            line << "stor ";
            line << binary_deserialize<size_t>(r);
            break;
        case cog::opcode::stori:
            line << "stori ";
            line << binary_deserialize<size_t>(r);
            break;
        case cog::opcode::jmp:
            line << "jmp ";
            printed_address = lazy_add_default_addr(binary_deserialize<size_t>(r));
            break;
        case cog::opcode::jal:
            line << "jal ";
            printed_address = lazy_add_default_addr(binary_deserialize<size_t>(r));
            break;
        case cog::opcode::bt:
            line << "bt ";
            printed_address = lazy_add_default_addr(binary_deserialize<size_t>(r));
            break;
        case cog::opcode::bf:
            line << "bf ";
            printed_address = lazy_add_default_addr(binary_deserialize<size_t>(r));
            break;
        case cog::opcode::call:
            line << "call ";
            line << verbs.get_verb(cog::verb_id(binary_deserialize<int>(r))).name;
            break;
        case cog::opcode::callv:
            line << "callv ";
            line << verbs.get_verb(cog::verb_id(binary_deserialize<int>(r))).name;
            break;
        case cog::opcode::ret:
            line << "ret";
            break;
        case cog::opcode::neg:
            line << "neg";
            break;
        case cog::opcode::lnot:
            line << "lnot";
            break;
        case cog::opcode::add:
            line << "add";
            break;
        case cog::opcode::sub:
            line << "sub";
            break;
        case cog::opcode::mul:
            line << "mul";
            break;
        case cog::opcode::div:
            line << "div";
            break;
        case cog::opcode::mod:
            line << "mod";
            break;
        case cog::opcode::bor:
            line << "bor";
            break;
        case cog::opcode::band:
            line << "band";
            break;
        case cog::opcode::bxor:
            line << "bxor";
            break;
        case cog::opcode::lor:
            line << "lor";
            break;
        case cog::opcode::land:
            line << "land";
            break;
        case cog::opcode::eq:
            line << "eq";
            break;
        case cog::opcode::ne:
            line << "ne";
            break;
        case cog::opcode::gt:
            line << "gt";
            break;
        case cog::opcode::ge:
            line << "ge";
            break;
        case cog::opcode::lt:
            line << "lt";
            break;
        case cog::opcode::le:
            line << "le";
            break;
        }

        lines.emplace(line_addr, std::make_tuple(line.str(), printed_address));
    }

    // Write out lines
    auto lbl_it = labels.begin();
    auto code_it = lines.begin();
    while(code_it != lines.end()) {
        if(lbl_it != labels.end() && lbl_it->first <= code_it->first) {
            std::cout << lbl_it->second << ":" << std::endl;
            ++lbl_it;
        }
        else {
            std::cout << "    " << std::get<0>(code_it->second);
            maybe_if(std::get<1>(code_it->second),
                     [&](size_t addr_print) {
                std::cout << labels[addr_print];
            });

            std::cout << std::endl;

            ++code_it;
        }
    }

    // LCOV_EXCL_START
    //
    // Branch targets after the last instruction are out of bounds.
    // This shouldn't happen normally.

    while(lbl_it != labels.end()) {
        std::cout << lbl_it->second << ":" << std::endl;
        ++lbl_it;
    }

    // LCOV_EXCL_STOP
}
