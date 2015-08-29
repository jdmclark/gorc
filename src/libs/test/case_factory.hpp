#pragma once

#include <string>
#include "utility/make_unique.hpp"

namespace test {

    class case_object;

    class case_factory {
    public:
        std::string const filename;
        int const line_number;

        case_factory(std::string const &f, int ln);
        virtual ~case_factory();

        virtual std::unique_ptr<case_object> create_case() = 0;
    };

    template <typename T>
    class specified_case_factory : public case_factory {
    public:
        specified_case_factory(std::string const &f, int ln)
            : case_factory(f, ln)
        {
            return;
        }

        virtual std::unique_ptr<case_object> create_case() override
        {
            return ::gorc::make_unique<T>();
        }
    };

}
