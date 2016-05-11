#pragma once

#include "text/json_input_stream.hpp"
#include "utility/time.hpp"
#include "jk/cog/script/message_type.hpp"
#include "jk/cog/script/value.hpp"
#include "jk/cog/script/source_type.hpp"

namespace gorc {

    class time_step_event;
    class send_linked_event;
    class quicksave_event;
    class quickload_event;

    class cog_scenario_event_visitor {
    public:
        virtual ~cog_scenario_event_visitor();

        virtual void visit(time_step_event const &e) = 0;
        virtual void visit(send_linked_event const &e) = 0;
        virtual void visit(quicksave_event const &e) = 0;
        virtual void visit(quickload_event const &e) = 0;
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

    class send_linked_event : public cog_scenario_event {
    public:
        cog::message_type msg;
        cog::value sender;
        cog::value source;
        cog::source_type st;
        cog::value param0;
        cog::value param1;
        cog::value param2;
        cog::value param3;

        send_linked_event(deserialization_constructor_tag, json_input_stream &);

        virtual void accept(cog_scenario_event_visitor &v) const override;
    };

    class quicksave_event : public cog_scenario_event {
    public:
        std::string key;

        quicksave_event(deserialization_constructor_tag, json_input_stream &);

        virtual void accept(cog_scenario_event_visitor &v) const override;
    };

    class quickload_event : public cog_scenario_event {
    public:
        std::string key;

        quickload_event(deserialization_constructor_tag, json_input_stream &);

        virtual void accept(cog_scenario_event_visitor &v) const override;
    };

}
