#include "subcommand_registry.hpp"
#include "log/log.hpp"

void gorc::subcommand_registry::add_subcommand(std::string const &name, subcommand &sub)
{
    subcommands.emplace(name, &sub);
}

int gorc::subcommand_registry::run(std::string const &name,
                                   abstract_argument_queue &args)
{
    auto it = subcommands.find(name);
    if(it == subcommands.end()) {
        LOG_FATAL(format("Unknown subcommand %s") % name);
    }

    return it->second->run(args);
}
