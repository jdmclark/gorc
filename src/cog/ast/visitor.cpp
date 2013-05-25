#include "visitor.h"
#include "framework/diagnostics/helper.h"

Gorc::Cog::AST::Visitor::Visitor(const std::string& name, Diagnostics::Report& report)
	: VisitorName(name), Report(report)
{
	return;
}

Gorc::Cog::AST::Visitor::~Visitor()
{
	return;
}

void Gorc::Cog::AST::Visitor::DefaultAction(const std::string& action, Node& n)
{
	Diagnostics::Helper::FeatureNotImplemented(Report, VisitorName, action);
}

ASTCLASS_ALL(AVDEFAULTACTION)
