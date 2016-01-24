#pragma once

#include "instance.hpp"
#include "sleep_record.hpp"
#include "cog/script/message_type.hpp"
#include "virtual_machine.hpp"
#include "cog/script/id.hpp"
#include "cog/script/verb_table.hpp"
#include "utility/service_registry.hpp"
#include "call_stack_frame.hpp"
#include <vector>
#include <memory>

namespace gorc {
    namespace cog {

        class executor {
        private:
            verb_table &verbs;
            virtual_machine vm;
            service_registry services;

            std::vector<std::unique_ptr<instance>> instances;
            std::vector<std::unique_ptr<sleep_record>> sleep_records;

        public:
            executor(verb_table &verbs);

            instance& create_instance(cog::script const &);
            instance& create_instance(cog::script const &, std::vector<value> const &);

            void add_sleep_record(std::unique_ptr<sleep_record> &&);

            maybe<call_stack_frame> create_message_frame(cog_id instance,
                                                         message_type msg,
                                                         value sender,
                                                         value source,
                                                         value param0,
                                                         value param1,
                                                         value param2,
                                                         value param3);

            void send_to_all(message_type msg,
                             value sender,
                             value source,
                             value param0,
                             value param1,
                             value param2,
                             value param3);

            void update(double dt);
        };

    }
}
