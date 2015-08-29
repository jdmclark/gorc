#pragma once

#include "suite.hpp"
#include "reporter.hpp"

#include <memory>
#include <string>

#include <iostream>

namespace test {

    class suite_factory {
    public:
        std::string const filename;
        int const line_number;

        suite_factory(std::string const &f, int ln);
        virtual ~suite_factory();

        virtual std::shared_ptr<suite> create_suite() = 0;
    };

    template <typename T>
    class specified_suite_factory : public suite_factory {
    public:
        specified_suite_factory(std::string const &f, int ln)
            : suite_factory(f, ln)
        {
            return;
        }

        virtual std::shared_ptr<suite> create_suite() override
        {
            return std::make_shared<T>();
        }
    };

}
