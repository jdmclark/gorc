#include "visitor.h"
#include "base/diagnostics/helper.h"

gorc::cog::ast::visitor::visitor(const std::string& name, diagnostics::report& report)
    : visitor_name(name), report(report)
{
    return;
}

gorc::cog::ast::visitor::~visitor()
{
    return;
}

void gorc::cog::ast::visitor::default_action(const std::string& action, node&)
{
    diagnostics::helper::feature_not_implemented(report, visitor_name, action);
}

ASTCLASS_ALL(AVDEFAULTACTION)
