#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include "maybe.hpp"
#include "strcat.hpp"

namespace gorc {

    class service_registry {
    private:
        maybe<service_registry const*> parent;
        std::unordered_map<std::type_index, void*> services;

    public:
        explicit service_registry(maybe<service_registry const*> parent = nothing);

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
            if(it != services.end()) {
                return *reinterpret_cast<ServiceT*>(it->second);
            }
            else if(parent.has_value()) {
                return parent.get_value()->get<ServiceT>();
            }
            else {
                throw std::runtime_error(strcat("service_registry::get service ",
                                                typeid(ServiceT).name(),
                                                " not registered"));
            }
        }
    };

}
