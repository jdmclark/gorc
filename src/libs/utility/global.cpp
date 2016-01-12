#include "global.hpp"
#include <unordered_map>
#include <mutex>
#include <iostream>

namespace {
    std::mutex global_registry_mutex;

    using global_map = std::unordered_map<std::type_index, std::shared_ptr<gorc::global>>;

    std::unique_ptr<global_map> global_registry;

    global_map& get_global_registry()
    {
        if(!global_registry) {
            global_registry = std::make_unique<global_map>();
        }

        return *global_registry;
    }
}

std::shared_ptr<gorc::global> gorc::global::reserve_global_dependency_with_factory(
        std::type_index tid,
        abstract_global_factory const &factory)
{
    auto &global_registry = get_global_registry();

    auto it = global_registry.find(tid);
    if(it == global_registry.end()) {
        it = global_registry.emplace(tid, factory.make_global()).first;
    }

    reserved_global_dependencies.push_back(it->second);

    return it->second;
}

gorc::global::~global()
{
    return;
}

gorc::abstract_global_factory::~abstract_global_factory()
{
    return;
}

std::shared_ptr<gorc::global> gorc::get_or_create_global(std::type_index tid,
                                                         abstract_global_factory const &factory)
{
    std::lock_guard<std::mutex> lk(global_registry_mutex);

    auto &global_registry = get_global_registry();

    auto it = global_registry.find(tid);
    if(it == global_registry.end()) {
        it = global_registry.emplace(tid, factory.make_global()).first;
    }

    return it->second;
}
