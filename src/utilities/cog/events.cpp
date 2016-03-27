#include "events.hpp"
#include "read_cog_value.hpp"
#include "text/json_specification.hpp"

using namespace gorc;

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

namespace {

    json_specification<send_linked_event> send_linked_event_spec(
            /* Members */
            {
                { "message", [](json_input_stream &f, send_linked_event &e) {
                        e.msg = cog::as_message_type(json_deserialize<std::string>(f));
                    }
                },
                { "sender", [](json_input_stream &f, send_linked_event &e) {
                        e.sender = read_cog_value(f);
                    }
                },
                { "source", [](json_input_stream &f, send_linked_event &e) {
                        e.source = read_cog_value(f);
                    }
                },
                { "source_type", [](json_input_stream &f, send_linked_event &e) {
                        e.st = cog::as_source_type(json_deserialize<std::string>(f));
                    }
                },
                { "param0", [](json_input_stream &f, send_linked_event &e) {
                        e.param0 = read_cog_value(f);
                    }
                },
                { "param1", [](json_input_stream &f, send_linked_event &e) {
                        e.param1 = read_cog_value(f);
                    }
                },
                { "param2", [](json_input_stream &f, send_linked_event &e) {
                        e.param2 = read_cog_value(f);
                    }
                },
                { "param3", [](json_input_stream &f, send_linked_event &e) {
                        e.param3 = read_cog_value(f);
                    }
                }
            },

            /* Required members */
            { "message", "sender", "source", "source_type" }
        );
}


gorc::send_linked_event::send_linked_event(deserialization_constructor_tag, json_input_stream &jis)
{
    json_deserialize_with_specification(jis, send_linked_event_spec, *this);
}

void gorc::send_linked_event::accept(cog_scenario_event_visitor &v) const
{
    v.visit(*this);
}
