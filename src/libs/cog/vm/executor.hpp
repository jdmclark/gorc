#pragma once

#include "instance.hpp"
#include "sleep_record.hpp"
#include "cog/script/message_type.hpp"
#include "virtual_machine.hpp"
#include "cog/script/id.hpp"
#include "cog/script/verb_table.hpp"
#include "utility/service_registry.hpp"
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

            void send_to_all(message_type);
        };

    }
}
