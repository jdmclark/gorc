#include "symbolext_visitor.h"
#include <algorithm>
#include <boost/format.hpp>
#include "framework/diagnostics/helper.h"
#include "symbolfield_visitor.h"
#include "cog/constants.h"

using namespace Gorc::Cog::AST;
using Gorc::Cog::Stages::SemanticAnalysis::SymbolExtensionVisitor;

SymbolExtensionVisitor::SymbolExtensionVisitor(Symbols::SymbolType type, Diagnostics::Report& report)
	: AST::Visitor("Stage2::SymbolExtensionVisitor", report), SymbolType(type) {
	// Set sensible default extensions.
	local = false;

	linkid_defined = false;
	linkid = Cog::Constants::DefaultLinkId;

	nolink = false;

	mask_defined = false;

	switch(SymbolType) {
	default:
		mask = Cog::Constants::DefaultMask;
		break;

	case Symbols::SymbolType::Sector:
		mask = Cog::Constants::DefaultSectorMask;
		break;

	case Symbols::SymbolType::Surface:
		mask = Cog::Constants::DefaultSurfaceMask;
		break;

	case Symbols::SymbolType::Thing:
		mask = Cog::Constants::DefaultThingMask;
		break;
	}

	return;
}

bool SymbolExtensionVisitor::ValidateExtension(const std::string& name, Text::Location& location) {
	if(name == "local") {
		if(local) {
			Diagnostics::Helper::ExtensionRedefinition(Report, VisitorName, name, location);
		}

		switch(SymbolType) {
		case Symbols::SymbolType::Message:
			Diagnostics::Helper::IllegalExtension(Report, VisitorName, name, location);
			break;

		default:
			break;
		}
	}
	else if(name == "linkid") {
		if(linkid_defined) {
			Diagnostics::Helper::ExtensionRedefinition(Report, VisitorName, name, location);
		}

		switch(SymbolType) {
		case Symbols::SymbolType::Sector:
		case Symbols::SymbolType::Surface:
		case Symbols::SymbolType::Thing:
			break;

		default:
			Diagnostics::Helper::IllegalExtension(Report, VisitorName, name, location);
		}
	}
	else if(name == "nolink") {
		if(nolink) {
			Diagnostics::Helper::ExtensionRedefinition(Report, VisitorName, name, location);
		}

		switch(SymbolType) {
		case Symbols::SymbolType::Cog:
		case Symbols::SymbolType::Sector:
		case Symbols::SymbolType::Surface:
		case Symbols::SymbolType::Thing:
			break;

		default:
			Diagnostics::Helper::IllegalExtension(Report, VisitorName, name, location);
		}
	}
	else if(name == "desc") {
		if(!desc.empty()) {
			Diagnostics::Helper::ExtensionRedefinition(Report, VisitorName, name, location);
		}

		switch(SymbolType) {
		case Symbols::SymbolType::Message:
			Diagnostics::Helper::IllegalExtension(Report, VisitorName, name, location);
			break;

		default:
			break;
		}
	}
	else if(name == "mask") {
		if(mask_defined) {
			Diagnostics::Helper::ExtensionRedefinition(Report, VisitorName, name, location);
		}

		switch(SymbolType) {
		case Symbols::SymbolType::Sector:
		case Symbols::SymbolType::Surface:
		case Symbols::SymbolType::Thing:
			break;

		default:
			Diagnostics::Helper::IllegalExtension(Report, VisitorName, name, location);
		}
	}
	else {
		Diagnostics::Helper::UnknownExtension(Report, VisitorName, name, location);
		return false;
	}

	return true;
}

void SymbolExtensionVisitor::VisitBareExtension(BareExtension& ext) {
	std::transform(ext.Name.begin(), ext.Name.end(), ext.Name.begin(), tolower);

	if(!ValidateExtension(ext.Name, ext.Location)) {
		return;
	}

	if(ext.Name == "linkid" || ext.Name == "desc" || ext.Name == "mask") {
		Diagnostics::Helper::ExtensionValueMissing(Report, VisitorName, ext.Name, ext.Location);
		return;
	}
	else if(ext.Name == "local") {
		local = true;
	}
	else if(ext.Name == "nolink") {
		nolink = true;
	}
}

void SymbolExtensionVisitor::VisitValuedExtension(ValuedExtension& ext) {
	std::transform(ext.Name.begin(), ext.Name.end(), ext.Name.begin(), tolower);

	if(!ValidateExtension(ext.Name, ext.Location)) {
		return;
	}

	SymbolFieldVisitor v(Report);
	ext.Value->Accept(v);

	if(ext.Name == "local" || ext.Name == "nolink") {
		Diagnostics::Helper::IllegalExtensionAssignment(Report, VisitorName, ext.Name, ext.Location);
		return;
	}
	else if(ext.Name == "linkid") {
		if(v.is_int) {
			linkid = v.int_value;
		}
		else {
			Diagnostics::Helper::ExtensionTypeMismatch(Report, VisitorName, ext.Name, ext.Location);
		}

		linkid_defined = true;
	}
	else if(ext.Name == "desc") {
		if(v.is_int) {
			desc = boost::str(boost::format("%d") % v.int_value);
		}
		else if(v.is_float) {
			desc = boost::str(boost::format("%f") % v.float_value);
		}
		else if(v.is_str) {
			desc = v.str_value;
		}
		else {
			Diagnostics::Helper::ExtensionTypeMismatch(Report, VisitorName, ext.Name, ext.Location);
		}
	}
	else if(ext.Name == "mask") {
		if(v.is_int) {
			mask = v.int_value;
		}
		else {
			Diagnostics::Helper::ExtensionTypeMismatch(Report, VisitorName, ext.Name, ext.Location);
		}

		mask_defined = true;
	}
}
