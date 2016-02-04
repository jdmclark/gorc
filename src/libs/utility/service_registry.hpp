#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include "strcat.hpp"

namespace gorc {

    class service_registry {
    private:
        std::unordered_map<std::type_index, void*> services;

    public:
        service_registry();

        template <typename ServiceT>
        void add(ServiceT &svc)
        {
            using BaseServiceT = typename std::decay<ServiceT>::type;
            std::type_index tid = typeid(BaseServiceT);
            bool rv = services.emplace(tid, reinterpret_cast<void*>(&svc)).second;
            if(!rv) {
                throw std::runtime_error(strcat("service_registry::add service ",
                                                typeid(ServiceT).name(),
                                                " multiple times"));
            }
        }

        template <typename ServiceT>
        void add_or_replace(ServiceT &svc)
        {
            using BaseServiceT = typename std::decay<ServiceT>::type;
            std::type_index tid = typeid(BaseServiceT);
            services[tid] = reinterpret_cast<void*>(&svc);
        }

        template <typename ServiceT>
        ServiceT& get() const
        {
            using BaseServiceT = typename std::decay<ServiceT>::type;
            std::type_index tid = typeid(BaseServiceT);
            auto it = services.find(tid);
            if(it == services.end()) {
                throw std::runtime_error(strcat("service_registry::get service ",
                                                typeid(ServiceT).name(),
                                                " not registered"));
            }

            return *reinterpret_cast<ServiceT*>(it->second);
        }
    };

}
