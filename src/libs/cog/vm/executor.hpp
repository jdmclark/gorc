#pragma once

#include "instance.hpp"
#include "sleep_record.hpp"
#include "cog/script/message_type.hpp"
#include "vm.hpp"
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
            vm virtual_machine;
            service_registry services;

            std::vector<std::unique_ptr<instance>> instances;
            std::vector<std::unique_ptr<sleep_record>> sleep_records;

        public:
            executor(verb_table &verbs);

            void create_instance(cog::script const &);

            void send_to_all(message_type);
        };

    }
}
