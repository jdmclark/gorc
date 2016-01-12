#include "subcommand_option.hpp"

gorc::subcommand_option::subcommand_option(subcommand_registry &reg,
                                           maybe<int> &binding)
    : reg(reg)
    , binding(binding)
{
    return;
}

void gorc::subcommand_option::reset()
{
    binding = nothing;
}

void gorc::subcommand_option::load_from_arg(std::string const &arg,
                                            abstract_argument_queue &args)
{
    binding = reg.run(arg, args);
}

bool gorc::subcommand_option::has_value() const
{
    return binding.has_value();
}

std::unique_ptr<gorc::subcommand_option> gorc::make_subcommand_option(
        gorc::subcommand_registry &reg,
        gorc::maybe<int> &binding)
{
    return std::make_unique<subcommand_option>(reg, binding);
}
