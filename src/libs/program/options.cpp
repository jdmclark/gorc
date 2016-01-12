#include "options.hpp"

bool gorc::options::has_value(std::string const &alias) const
{
    auto const &opt = get_option<abstract_option>(alias);
    return opt.has_value();
}

bool gorc::options::has_bare_value() const
{
    return bare_option && bare_option->has_value();
}

void gorc::options::add_alias(std::string const &option,
                              std::string const &alias)
{
    auto it = opt_map.find(option);
    if(it == opt_map.end()) {
        LOG_FATAL(format("Option %s has not been defined") % option);
    }

    auto alias_map_res = alias_map.emplace(alias, it->second);
    if(!alias_map_res.second) {
        LOG_FATAL(format("Option alias %s has already been defined") % alias);
    }
}

void gorc::options::insert(std::unique_ptr<abstract_option>&& opt)
{
    // Sanity check name
    if(std::find_if(opt->name.begin(),
                    opt->name.end(),
                    [](char ch) { return std::isspace(ch); }) != opt->name.end()) {
        LOG_FATAL(format("Option %s name cannot contain whitespace") % opt->name);
    }

    opts.push_back(std::forward<std::unique_ptr<abstract_option>>(opt));
    abstract_option *reg_opt = opts.back().get();

    auto opt_map_res = opt_map.emplace(reg_opt->name, reg_opt);
    if(!opt_map_res.second) {
        LOG_FATAL(format("Option %s has already been defined") % reg_opt->name);
    }

    opt_map.emplace(reg_opt->name, reg_opt);

    auto full_name = str(format("--%s") % reg_opt->name);
    add_alias(reg_opt->name, full_name);
}

void gorc::options::insert_bare(std::unique_ptr<abstract_bare_option> &&opt)
{
    bare_option = std::forward<std::unique_ptr<abstract_bare_option>>(opt);
}

void gorc::options::load_from_arg_queue(abstract_argument_queue &arg_list)
{
    // Reset all opts.
    if(bare_option) {
        bare_option->reset();
    }

    for(auto &opt : opts) {
        opt->reset();
    }

    while(!arg_list.empty()) {
        auto arg = arg_list.peek();
        arg_list.pop();

        // Check if it's a bare option
        if(bare_option && !begins_with(arg, "-")) {
            bare_option->load_from_arg(arg, arg_list);
            continue;
        }

        auto it = alias_map.find(arg);
        if(it == alias_map.end()) {
            LOG_FATAL(format("Unrecognized option %s") % arg);
        }
        else {
            it->second->load_from_arg_list(arg_list);
        }
    }

    // Evaluate constraints
    for(auto const &constraint : constraints) {
        constraint->check_constraint(*this);
    }
}
