#pragma once

#include "instance.hpp"
#include "sleep_record.hpp"
#include "jk/cog/script/message_type.hpp"
#include "virtual_machine.hpp"
#include "content/id.hpp"
#include "jk/cog/script/verb_table.hpp"
#include "utility/service_registry.hpp"
#include "call_stack_frame.hpp"
#include "content/asset_ref.hpp"
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"
#include <vector>
#include <memory>
#include <map>
#include <tuple>

namespace gorc {
    namespace cog {

        namespace detail {
            struct executor_link_comp {
                bool operator()(value left, value right) const;
            };

            struct executor_wait_comp {
                bool operator()(std::tuple<message_type, value> const&,
                                std::tuple<message_type, value> const&) const;
            };

            struct executor_gi_comp {
                bool operator()(asset_ref<script> left, asset_ref<script> right) const;
            };
        }

        class executor_linkage {
        public:
            flag_set<source_type> mask;
            cog_id instance_id;
            value sender_link_id;

            executor_linkage(flag_set<source_type> mask,
                             cog_id instance_id,
                             value sender_link_id);
            executor_linkage(deserialization_constructor_tag, binary_input_stream &);

            void binary_serialize_object(binary_output_stream &) const;
        };

        class executor {
        private:
            verb_table &verbs;
            virtual_machine vm;
            service_registry services;

            std::vector<std::unique_ptr<instance>> instances;
            std::vector<std::unique_ptr<sleep_record>> sleep_records;
            std::multimap<std::tuple<message_type, value>,
                          std::unique_ptr<continuation>,
                          detail::executor_wait_comp> wait_records;
            std::multimap<value, executor_linkage, detail::executor_link_comp> linkages;
            std::map<asset_ref<script>, cog_id, detail::executor_gi_comp> global_instance_map;

            cog_id master_cog;

            void add_linkage(cog_id id, instance const &inst);

        public:
            explicit executor(verb_table &verbs);
            executor(deserialization_constructor_tag, binary_input_stream &);

            void binary_serialize_object(binary_output_stream &) const;

            cog_id create_instance(asset_ref<cog::script>);
            cog_id create_instance(asset_ref<cog::script>, std::vector<value> const &);
            cog_id create_global_instance(asset_ref<cog::script>);

            instance& get_instance(cog_id instance_id);

            void add_sleep_record(std::unique_ptr<sleep_record> &&);
            void add_wait_record(message_type msg, value sender, std::unique_ptr<continuation>&&);

            maybe<call_stack_frame> create_message_frame(cog_id instance,
                                                         message_type msg,
                                                         value sender,
                                                         value sender_id,
                                                         value source,
                                                         value param0,
                                                         value param1,
                                                         value param2,
                                                         value param3);

            value send(cog_id instance,
                       message_type msg,
                       value sender,
                       value sender_id,
                       value source,
                       value param0 = value(),
                       value param1 = value(),
                       value param2 = value(),
                       value param3 = value());

            void send_to_all(message_type msg,
                             value sender,
                             value sender_id,
                             value source,
                             value param0 = value(),
                             value param1 = value(),
                             value param2 = value(),
                             value param3 = value());

            void send_to_linked(message_type msg,
                                value sender,
                                value source,
                                source_type st,
                                value param0 = value(),
                                value param1 = value(),
                                value param2 = value(),
                                value param3 = value());

            void set_master_cog(cog_id);
            cog_id get_master_cog() const;

            void update(time_delta dt);
        };

    }
}
