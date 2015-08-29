#include "switch_option.hpp"
#include "log/log.hpp"
#include "utility/make_unique.hpp"

gorc::switch_option::switch_option(std::string const &name, bool &binding)
    : abstract_option(name)
    , binding(binding)
{
    return;
}

void gorc::switch_option::reset()
{
    binding = false;
}

void gorc::switch_option::load_from_arg_list(abstract_argument_queue &)
{
    if(binding) {
        // Switch is already set, and is being re-set. Raise a warning.
        LOG_WARNING(format("Option %s is already set") % name);
    }

    binding = true;
}

bool gorc::switch_option::has_value() const
{
    return binding;
}

std::unique_ptr<gorc::abstract_option> gorc::make_switch_option(std::string const &name,
                                                                bool &binding)
{
    return make_unique<switch_option>(name, binding);
}
