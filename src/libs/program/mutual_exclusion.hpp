#pragma once

#include "option_constraint.hpp"
#include <vector>
#include <string>
#include <initializer_list>

namespace gorc {

    class mutual_exclusion : public option_constraint {
    private:
        std::vector<std::string> exclusive_options;
        int min_set;
        int max_set;

    public:
        mutual_exclusion(std::vector<std::string> const &exclusive_options,
                         int min_set = 0,
                         int max_set = 1);

        virtual void check_constraint(options const &opt) const override;
    };

}
