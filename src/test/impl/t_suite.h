#pragma once

namespace test {
    class suite {
    public:
        virtual ~suite() { }

        virtual case_vector& get_case_factories() = 0;
        virtual case_map& get_case_factory_map() = 0;
    };
}
