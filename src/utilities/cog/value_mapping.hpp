#pragma once

#include "jk/cog/vm/default_value_mapping.hpp"
#include "scenario.hpp"

namespace gorc {

    class cog_scenario_value_mapping : public cog::default_value_mapping {
    private:
        cog_scenario const &scn;

    public:
        cog_scenario_value_mapping(cog_scenario const &scn);

        virtual cog::value operator()(cog::value_type type,
                                      std::string const &value) const override;
    };

}
