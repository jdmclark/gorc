#pragma once

#include "build/boc-build/engine/entity.hpp"
#include <string>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <set>
#include <vector>

class mock_entity : public gorc::entity {
private:
    std::string const name_value;

public:
    mock_entity(std::string const &name);

    bool is_dirty_value = false;
    std::unordered_set<gorc::entity*> dependencies_value;

    virtual std::string const& name() override;
    virtual std::unordered_set<gorc::entity*> const& dependencies() override;

    virtual bool is_dirty() override;

    bool will_update_succeed = true;
    virtual bool update(gorc::service_registry const &) override;

    virtual void serialize(gorc::entity_output_stream &) override;
};

std::unordered_map<std::string, std::unique_ptr<gorc::entity>> construct_mock_entity_graph(
        std::vector<std::tuple<char const *, char const *>> const &edges,
        std::unordered_set<std::string> const &dirty_nodes = std::unordered_set<std::string>(),
        std::unordered_set<std::string> const &fail_nodes = std::unordered_set<std::string>());

template <typename RangeT>
std::set<std::string> mock_entities_to_strings(RangeT const &rng)
{
    std::set<std::string> rv;
    for(gorc::entity *ent : rng) {
        rv.emplace(ent->name());
    }

    return rv;
}
