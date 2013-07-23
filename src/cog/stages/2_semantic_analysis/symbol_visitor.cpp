#include "symbol_visitor.h"
#include "symbolfield_visitor.h"
#include "symbolext_visitor.h"
#include "framework/diagnostics/helper.h"

#include <algorithm>
#include <boost/format.hpp>

using namespace Gorc::Cog::AST;
using Gorc::Cog::Stages::SemanticAnalysis::SymbolVisitor;

SymbolVisitor::SymbolVisitor(Symbols::SymbolTable& symbolTable,
	const std::unordered_set<std::string>& SeenLabels, Diagnostics::Report& report)
	: AST::Visitor("Stage2::SymbolVisitor", report), SymbolTable(symbolTable), SeenLabels(SeenLabels) {
	return;
}

void SymbolVisitor::VisitSymbol(Symbol& symbol) {
	Symbols::SymbolType type = GetType(symbol);

	VM::Value defaultValue;

	switch(type) {
	case Symbols::SymbolType::Flex:
	case Symbols::SymbolType::Float:
		defaultValue = 0.0f;
		break;

	case Symbols::SymbolType::Int:
		defaultValue = 0;
		break;

	case Symbols::SymbolType::Vector:
		defaultValue = Math::Zero<3>();
		break;

	default:
	case Symbols::SymbolType::Ai:
	case Symbols::SymbolType::Keyframe:
	case Symbols::SymbolType::Material:
	case Symbols::SymbolType::Model:
	case Symbols::SymbolType::Sound:
	case Symbols::SymbolType::Template:
	case Symbols::SymbolType::Cog:
	case Symbols::SymbolType::Message:
	case Symbols::SymbolType::Sector:
	case Symbols::SymbolType::Surface:
	case Symbols::SymbolType::String:
	case Symbols::SymbolType::Thing:
		break;
	}

	// Convert symbol name to lowercase for processing.
	std::transform(symbol.Name.begin(), symbol.Name.end(), symbol.Name.begin(), tolower);

	// Visit extension
	SymbolExtensionVisitor w(type, Report);
	for(auto& ext : *symbol.Extensions) {
		ext->Accept(w);
	}

	if(SymbolTable.IsSymbolDefined(symbol.Name)) {
		Diagnostics::Helper::SymbolRedefinition(Report, VisitorName, symbol.Name, symbol.Location);
		SymbolTable.ReplaceSymbol(type, symbol.Name, defaultValue, w.local, w.desc, w.mask, w.linkid, w.nolink);
	}
	else {
		SymbolTable.AddSymbol(type, symbol.Name, defaultValue, w.local, w.desc, w.mask, w.linkid, w.nolink);

		if(type == Symbols::SymbolType::Message) {
			if(SeenLabels.find(symbol.Name) == SeenLabels.end()) {
				Diagnostics::Helper::MissingExport(Report, VisitorName, symbol.Name, symbol.Location);
			}
		}
	}
}

void SymbolVisitor::VisitValuedSymbol(ValuedSymbol& symbol) {
	SymbolFieldVisitor v(Report);
	symbol.Value->Accept(v);

	Symbols::SymbolType type = GetType(symbol);

	VM::Value defaultValue(0);

	switch(type) {
	case Symbols::SymbolType::Flex:
	case Symbols::SymbolType::Float:
		if(!v.is_numeric) {
			Diagnostics::Helper::TypeMismatch(Report, VisitorName, symbol.Location);
		}
		else {
			defaultValue = VM::Value(v.float_value);
		}
		break;

	case Symbols::SymbolType::Int:
		if(!v.is_numeric) {
			Diagnostics::Helper::TypeMismatch(Report, VisitorName, symbol.Location);
		}
		else {
			defaultValue = VM::Value(v.int_value);
		}
		break;
	
	case Symbols::SymbolType::Ai:
	case Symbols::SymbolType::Keyframe:
	case Symbols::SymbolType::Material:
	case Symbols::SymbolType::Model:
	case Symbols::SymbolType::Sound:
	case Symbols::SymbolType::Template:
		if(!v.is_str) {
			Diagnostics::Helper::TypeMismatch(Report, VisitorName, symbol.Location);
		}
		else {
			defaultValue = VM::Value(SymbolTable.AddString(v.str_value));
		}
		break;

	default:
	case Symbols::SymbolType::Cog:
	case Symbols::SymbolType::Message:
	case Symbols::SymbolType::Sector:
	case Symbols::SymbolType::String:
	case Symbols::SymbolType::Surface:
	case Symbols::SymbolType::Thing:
	case Symbols::SymbolType::Vector:
		Diagnostics::Helper::IllegalAssignment(Report, VisitorName, symbol.Location);
		break;
	}

	// Convert symbol name to lowercase for processing.
	std::transform(symbol.Name.begin(), symbol.Name.end(), symbol.Name.begin(), tolower);

	// Visit extension
	SymbolExtensionVisitor w(type, Report);
	for(auto& ext : *symbol.Extensions) {
		ext->Accept(w);
	}

	if(SymbolTable.IsSymbolDefined(symbol.Name)) {
		Diagnostics::Helper::SymbolRedefinition(Report, VisitorName, symbol.Name, symbol.Location);
		SymbolTable.ReplaceSymbol(type, symbol.Name, defaultValue, w.local, w.desc, w.mask, w.linkid, w.nolink);
	}
	else {
		SymbolTable.AddSymbol(type, symbol.Name, defaultValue, w.local, w.desc, w.mask, w.linkid, w.nolink);
	}
}

Gorc::Cog::Symbols::SymbolType SymbolVisitor::GetType(Symbol& symbol) {
	// Convert symbol type to lowercase for processing.
	std::transform(symbol.Type.begin(), symbol.Type.end(), symbol.Type.begin(), tolower);

	if(symbol.Type == "ai") {
		return Symbols::SymbolType::Ai;
	}
	else if(symbol.Type == "cog") {
		return Symbols::SymbolType::Cog;
	}
	else if(symbol.Type == "flex") {
		return Symbols::SymbolType::Flex;
	}
	else if(symbol.Type == "float") {
		return Symbols::SymbolType::Float;
	}
	else if(symbol.Type == "int") {
		return Symbols::SymbolType::Int;
	}
	else if(symbol.Type == "keyframe") {
		return Symbols::SymbolType::Keyframe;
	}
	else if(symbol.Type == "material") {
		return Symbols::SymbolType::Material;
	}
	else if(symbol.Type == "message") {
		return Symbols::SymbolType::Message;
	}
	else if(symbol.Type == "model") {
		return Symbols::SymbolType::Model;
	}
	else if(symbol.Type == "sector") {
		return Symbols::SymbolType::Sector;
	}
	else if(symbol.Type == "sound") {
		return Symbols::SymbolType::Sound;
	}
	else if(symbol.Type == "string") {
		return Symbols::SymbolType::String;
	}
	else if(symbol.Type == "surface") {
		return Symbols::SymbolType::Surface;
	}
	else if(symbol.Type == "template") {
		return Symbols::SymbolType::Template;
	}
	else if(symbol.Type == "thing") {
		return Symbols::SymbolType::Thing;
	}
	else if(symbol.Type == "vector") {
		return Symbols::SymbolType::Vector;
	}
	else {
		Diagnostics::Helper::UnknownType(Report, VisitorName, symbol.Type, symbol.Location);

		return Symbols::SymbolType::Int;
	}
}
