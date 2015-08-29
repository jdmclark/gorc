#pragma once

#include <vector>
#include <utility>
#include <unordered_map>
#include <memory>

#include "case_factory.hpp"
#include "utility/global.hpp"

namespace test {

    using case_vector = std::vector<std::pair<std::string, std::shared_ptr<case_factory>>>;
    using case_map = std::unordered_map<std::string, std::shared_ptr<case_factory>>;

    template <typename T>
    class case_registry : public ::gorc::global {
        template <typename GlobalT> friend class ::gorc::global_factory;

    private:
        case_registry()
            : locked(false)
        {
            return;
        }

    public:
        bool locked;
        case_vector factories;
        case_map factory_map;
    };

    template <typename T, typename C>
    void register_case(const std::string& s,
                       const std::string& file_name,
                       int line_number)
    {
        auto &reg = *::gorc::get_global<case_registry<T>>();

        if(!reg.locked) {
            auto case_fac = std::make_shared<specified_case_factory<C>>(file_name, line_number);
            auto pair = std::make_pair(s, case_fac);

            reg.factories.push_back(pair);
            reg.factory_map.insert(pair);
        }

        return;
    }

    template <typename T> void lock_case_registry()
    {
        ::gorc::get_global<case_registry<T>>()->locked = true;
    }

}
