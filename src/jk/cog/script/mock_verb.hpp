#pragma once

#include "verb.hpp"

namespace gorc {
    namespace cog {

        class mock_verb : public verb {
        public:
            using verb::verb;

            virtual value invoke(stack &, service_registry &, bool) const override;
        };

    }
}
