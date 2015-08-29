#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include "utility/global.hpp"
#include "suite_factory.hpp"

namespace test {

typedef std::unordered_map<std::string, std::shared_ptr<suite_factory>> suite_map;

class suite_registry : public ::gorc::global {
    template <typename GlobalT> friend class ::gorc::global_factory;

private:
    suite_registry();

public:
    suite_map factories;
};

template <typename T>
void register_suite(std::string const &s,
                    std::string const &file_name,
                    int line_number)
{
    ::gorc::get_global<suite_registry>()->factories.emplace(
            s, std::make_shared<specified_suite_factory<T>>(file_name, line_number));
    return;
}

}
