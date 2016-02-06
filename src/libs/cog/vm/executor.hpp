#pragma once

#include "instance.hpp"
#include "sleep_record.hpp"
#include "cog/script/message_type.hpp"
#include "virtual_machine.hpp"
#include "cog/script/id.hpp"
#include "cog/script/verb_table.hpp"
#include "utility/service_registry.hpp"
#include "call_stack_frame.hpp"
#include "content/asset_ref.hpp"
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"
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
            explicit executor(verb_table &verbs);
            executor(deserialization_constructor_tag, binary_input_stream &);

            void binary_serialize_object(binary_output_stream &) const;

            instance& create_instance(asset_ref<cog::script>);
            instance& create_instance(asset_ref<cog::script>, std::vector<value> const &);

            instance& get_instance(cog_id instance_id);

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

            void update(time_delta dt);
        };

    }
}
