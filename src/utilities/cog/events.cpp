#include "events.hpp"

gorc::cog_scenario_event_visitor::~cog_scenario_event_visitor()
{
    return;
}

gorc::cog_scenario_event::~cog_scenario_event()
{
    return;
}

gorc::time_step_event::time_step_event(deserialization_constructor_tag, json_input_stream &jis)
    : step(json_deserialize<double>(jis))
{
    return;
}

void gorc::time_step_event::accept(cog_scenario_event_visitor &v) const
{
    v.visit(*this);
}
