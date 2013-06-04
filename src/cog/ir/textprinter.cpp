#include "textprinter.h"

using Gorc::Cog::IR::TextPrinter;

Gorc::Cog::IR::TextPrinter::TextPrinter(std::ostream& os)
	: stream(os) {
	return;
}

void TextPrinter::Backpatch() {
	return;
}

void TextPrinter::writeValue(const VM::Value& value) {
	switch(value.GetType()) {
	case VM::Type::Boolean:
		if(static_cast<bool>(value)) {
			stream << "boolean(true)";
		}
		else {
			stream << "boolean(false)";
		}
		break;

	case VM::Type::Float:
		stream << "float(" << static_cast<float>(value) << ")";
		break;

	case VM::Type::Integer:
		stream << "integer(" << static_cast<int>(value) << ")";
		break;

	case VM::Type::String:
		stream << "string(" << static_cast<const char*>(value) << ")";
		break;

	case VM::Type::Vector: {
			Math::Vector<3> v = static_cast<Math::Vector<3>>(value);
			stream << "vector(" << Math::Get<Math::X>(v) << ", " << Math::Get<Math::Y>(v) << ", " << Math::Get<Math::Z>(v) << ")";
		}
		break;

	case VM::Type::Void:
		stream << "void()";
		break;
	}
}

void TextPrinter::Comment(const std::string& msg) {
	stream << "; " << msg << std::endl;
}

void TextPrinter::Label(const std::string& name) {
	stream << name << ":" << std::endl;
}

void TextPrinter::Nop() {
	stream << "\t\tnop" << std::endl;
}

void TextPrinter::Copy() {
	stream << "\t\tcopy" << std::endl;
}

void TextPrinter::Const(const VM::Value& value) {
	stream << "\t\tconst ";

	writeValue(value);

	stream << std::endl;
}

void TextPrinter::Load(const std::string& symbol) {
	stream << "\t\tload symbol(" << symbol << ")" << std::endl;
}

void TextPrinter::LoadI(const std::string& symbol) {
	stream << "\t\tloadi symbol(" << symbol << ")" << std::endl;
}

void TextPrinter::Store(const std::string& symbol) {
	stream << "\t\tstore symbol(" << symbol << ")" << std::endl;
}

void TextPrinter::StoreI(const std::string& symbol) {
	stream << "\t\tstorei symbol(" << symbol << ")" << std::endl;
}

void TextPrinter::Jmp(const std::string& label) {
	stream << "\t\tjmp " << label << std::endl;
}

void TextPrinter::Jal(const std::string& label) {
	stream << "\t\tjal " << label << std::endl;
}

void TextPrinter::Bt(const std::string& label) {
	stream << "\t\tbt " << label << std::endl;
}

void TextPrinter::Bf(const std::string& label) {
	stream << "\t\tbf " << label << std::endl;
}

void TextPrinter::Call(const std::string& verb) {
	stream << "\t\tcall " << verb << std::endl;
}

void TextPrinter::CallV(const std::string& verb) {
	stream << "\t\tcallv " << verb << std::endl;
}

void TextPrinter::Ret() {
	stream << "\t\tret" << std::endl;
}

void TextPrinter::Neg() {
	stream << "\t\tneg" << std::endl;
}

void TextPrinter::Add() {
	stream << "\t\tadd" << std::endl;
}

void TextPrinter::Sub() {
	stream << "\t\tsub" << std::endl;
}

void TextPrinter::Mul() {
	stream << "\t\tmul" << std::endl;
}

void TextPrinter::Div() {
	stream << "\t\tdiv" << std::endl;
}

void TextPrinter::Mod() {
	stream << "\t\tmod" << std::endl;
}

void TextPrinter::And() {
	stream << "\t\tand" << std::endl;
}

void TextPrinter::Or() {
	stream << "\t\tor" << std::endl;
}

void TextPrinter::Xor() {
	stream << "\t\txor" << std::endl;
}

void TextPrinter::LNot() {
	stream << "\t\tlnot" << std::endl;
}

void TextPrinter::LAnd() {
	stream << "\t\tland" << std::endl;
}

void TextPrinter::LOr() {
	stream << "\t\tlor" << std::endl;
}

void TextPrinter::CGt() {
	stream << "\t\tcgt" << std::endl;
}

void TextPrinter::CGeq() {
	stream << "\t\tcgeq" << std::endl;
}

void TextPrinter::CLt() {
	stream << "\t\tclt" << std::endl;
}

void TextPrinter::CLeq() {
	stream << "\t\tcleq" << std::endl;
}

void TextPrinter::CEq() {
	stream << "\t\tceq" << std::endl;
}

void TextPrinter::CNeq() {
	stream << "\t\tcneq" << std::endl;
}
