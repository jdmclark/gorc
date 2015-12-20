#include "symbols.hpp"
#include "log/log.hpp"
#include <unordered_map>
#include "utility/runtime_assert.hpp"

namespace {
    std::unordered_map<std::string, std::unique_ptr<gorc::builtin>> builtins;
}

gorc::builtin::builtin(size_t args, std::function<shvalue(arglist const &)> code)
    : args(args)
    , code(code)
{
    return;
}

void gorc::register_builtin(std::string const &name, std::unique_ptr<builtin> &&obj)
{
    runtime_assert(builtins.emplace(name, std::forward<std::unique_ptr<builtin>>(obj)).second,
                   str(format("builtin '%s' redefinition") % name));
}

gorc::maybe<gorc::builtin*> gorc::get_builtin(std::string const &name)
{
    auto it = builtins.find(name);
    if(it != builtins.end()) {
        return it->second.get();
    }

    return nothing;
}

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
