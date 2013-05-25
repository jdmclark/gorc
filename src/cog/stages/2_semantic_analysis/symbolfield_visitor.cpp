#include "symbolfield_visitor.h"

using namespace Gorc::Cog::AST;
using Gorc::Cog::Stages::SemanticAnalysis::SymbolFieldVisitor;

SymbolFieldVisitor::SymbolFieldVisitor(Diagnostics::Report& report)
	: AST::Visitor("Stage2::SymbolFieldVisitor", report),
	float_value(0.0f), is_float(false), int_value(0), is_int(false), is_numeric(false), is_str(false) {
	return;
}

void SymbolFieldVisitor::VisitStringFragmentField(StringFragmentField& v) {
	is_str = true;
	str_value = v.Value;
}

void SymbolFieldVisitor::VisitIntegerField(IntegerField& v) {
	is_int = true;
	is_numeric = true;
	int_value = v.Value;
	float_value = static_cast<float>(v.Value);
}

void SymbolFieldVisitor::VisitFloatField(FloatField& v) {
	is_float = true;
	is_numeric = true;
	float_value = v.Value;
	int_value = static_cast<int>(v.Value);
}

