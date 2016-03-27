#pragma once

#include "text/json_input_stream.hpp"
#include "utility/time.hpp"

namespace gorc {

    class time_step_event;

    class cog_scenario_event_visitor {
    public:
        virtual ~cog_scenario_event_visitor();

        virtual void visit(time_step_event const &e) = 0;
    };

    class cog_scenario_event {
    public:
        virtual ~cog_scenario_event();

        virtual void accept(cog_scenario_event_visitor &v) const = 0;
    };

    class time_step_event : public cog_scenario_event {
    public:
        time_delta step;

        time_step_event(deserialization_constructor_tag, json_input_stream &);

        virtual void accept(cog_scenario_event_visitor &v) const override;
    };

}
