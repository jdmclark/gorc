#include "text_printer.h"

using gorc::cog::ir::textPrinter;

gorc::cog::ir::textPrinter::textPrinter(std::ostream& os)
    : stream(os) {
    return;
}

void textPrinter::backpatch() {
    return;
}

void textPrinter::writevalue(const vm::value& value) {
    switch(value.get_type()) {
    case vm::type::boolean:
        if(static_cast<bool>(value)) {
            stream << "boolean(true)";
        }
        else {
            stream << "boolean(false)";
        }
        break;

    case vm::type::floating:
        stream << "float(" << static_cast<float>(value) << ")";
        break;

    case vm::type::integer:
        stream << "integer(" << static_cast<int>(value) << ")";
        break;

    case vm::type::string:
        stream << "string(" << static_cast<const char*>(value) << ")";
        break;

    case vm::type::vector: {
            vector<3> v = static_cast<vector<3>>(value);
            stream << "vector(" << get<0>(v) << ", " << get<1>(v) << ", " << get<2>(v) << ")";
        }
        break;

    case vm::type::nothing:
        stream << "void()";
        break;

    case vm::type::dynamic:
        stream << "dynamic(?)";
        break;
    }
}

void textPrinter::comment(const std::string& msg) {
    stream << "; " << msg << std::endl;
}

void textPrinter::label(const std::string& name) {
    stream << name << ":" << std::endl;
}

void textPrinter::nop() {
    stream << "\t\tnop" << std::endl;
}

void textPrinter::copy() {
    stream << "\t\tcopy" << std::endl;
}

void textPrinter::constant(const vm::value& value) {
    stream << "\t\tconst ";

    writevalue(value);

    stream << std::endl;
}

void textPrinter::load(const std::string& symbol) {
    stream << "\t\tload symbol(" << symbol << ")" << std::endl;
}

void textPrinter::loadi(const std::string& symbol) {
    stream << "\t\tloadi symbol(" << symbol << ")" << std::endl;
}

void textPrinter::store(const std::string& symbol) {
    stream << "\t\tstore symbol(" << symbol << ")" << std::endl;
}

void textPrinter::storei(const std::string& symbol) {
    stream << "\t\tstorei symbol(" << symbol << ")" << std::endl;
}

void textPrinter::jmp(const std::string& label) {
    stream << "\t\tjmp " << label << std::endl;
}

void textPrinter::jal(const std::string& label) {
    stream << "\t\tjal " << label << std::endl;
}

void textPrinter::bt(const std::string& label) {
    stream << "\t\tbt " << label << std::endl;
}

void textPrinter::bf(const std::string& label) {
    stream << "\t\tbf " << label << std::endl;
}

void textPrinter::call(const std::string& verb) {
    stream << "\t\tcall " << verb << std::endl;
}

void textPrinter::callv(const std::string& verb) {
    stream << "\t\tcallv " << verb << std::endl;
}

void textPrinter::ret() {
    stream << "\t\tret" << std::endl;
}

void textPrinter::neg() {
    stream << "\t\tneg" << std::endl;
}

void textPrinter::add() {
    stream << "\t\tadd" << std::endl;
}

void textPrinter::sub() {
    stream << "\t\tsub" << std::endl;
}

void textPrinter::mul() {
    stream << "\t\tmul" << std::endl;
}

void textPrinter::div() {
    stream << "\t\tdiv" << std::endl;
}

void textPrinter::mod() {
    stream << "\t\tmod" << std::endl;
}

void textPrinter::band() {
    stream << "\t\tand" << std::endl;
}

void textPrinter::bor() {
    stream << "\t\tor" << std::endl;
}

void textPrinter::bxor() {
    stream << "\t\txor" << std::endl;
}

void textPrinter::lnot() {
    stream << "\t\tlnot" << std::endl;
}

void textPrinter::land() {
    stream << "\t\tland" << std::endl;
}

void textPrinter::lor() {
    stream << "\t\tlor" << std::endl;
}

void textPrinter::cgt() {
    stream << "\t\tcgt" << std::endl;
}

void textPrinter::cgeq() {
    stream << "\t\tcgeq" << std::endl;
}

void textPrinter::clt() {
    stream << "\t\tclt" << std::endl;
}

void textPrinter::cleq() {
    stream << "\t\tcleq" << std::endl;
}

void textPrinter::ceq() {
    stream << "\t\tceq" << std::endl;
}

void textPrinter::cneq() {
    stream << "\t\tcneq" << std::endl;
}
