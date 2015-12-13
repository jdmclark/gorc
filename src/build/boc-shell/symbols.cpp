#include "symbols.hpp"
#include "log/log.hpp"
#include <unordered_map>

namespace {
    std::unordered_map<std::string, gorc::func_declaration_statement*> functions;
}

void gorc::register_function(gorc::func_declaration_statement &s)
{
    if(!functions.emplace(s.name->value, &s).second) {
        diagnostic_context dc(s.location);
        LOG_FATAL(format("function '%s' redefinition") % s.name->value);
    }
}

gorc::func_declaration_statement& gorc::get_function(std::string const &name)
{
    auto it = functions.find(name);
    if(it != functions.end()) {
        return *it->second;
    }

    LOG_FATAL(format("function '%s' is not defined") % name);
}
