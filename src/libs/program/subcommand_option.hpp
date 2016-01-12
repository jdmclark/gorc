#pragma once

#include "abstract_bare_option.hpp"
#include "subcommand_registry.hpp"
#include "utility/maybe.hpp"
#include <unordered_map>
#include <string>
#include <memory>

namespace gorc {

    class subcommand_option : public abstract_bare_option {
    private:
        subcommand_registry &reg;
        maybe<int> &binding;

    public:
        subcommand_option(subcommand_registry &reg, maybe<int> &binding);

        virtual void reset() override;
        virtual void load_from_arg(std::string const &arg,
                                   abstract_argument_queue &) override;
        virtual bool has_value() const override;
    };

    std::unique_ptr<subcommand_option> make_subcommand_option(subcommand_registry &,
                                                              maybe<int> &);

}
