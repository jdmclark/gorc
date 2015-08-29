#pragma once

#include "option_constraint.hpp"
#include <string>
#include <vector>

namespace gorc {

    class required_option : public option_constraint {
    private:
        std::vector<std::string> names;

    public:
        required_option(std::string const &name);
        required_option(std::vector<std::string> const &names);

        virtual void check_constraint(options const &opt) const override;
    };

}
