#include "component_system.h"
#include "fourcc.h"
#include "base/io/exception.h"

gorc::utility::component_system::entity_iterator::entity_iterator(const pool<entity, 1024>::iterator& it) : it(it) {
    return;
}

gorc::utility::component_system::entity_const_iterator::entity_const_iterator(const pool<entity, 1024>::const_iterator& it) : it(it) {
    return;
}

gorc::utility::component_system::pool_container::~pool_container() {
    return;
}

void gorc::utility::component_system::serialize(io::binary_output_stream& f) const {

    // Write header
    fourcc expected_fourcc("BECS");
    uint32_t expected_version = 1;

    io::serialize(f, expected_fourcc);
    io::serialize<uint32_t>(f, expected_version);

    // Count spawnable entities:
    int32_t max_eid = -1;
    for(auto eid : all_entities()) {
        int32_t cur_eid = static_cast<int32_t>(eid);
        max_eid = std::max(max_eid, cur_eid);
    }

    // Record number of entities to spawn
    io::serialize<int32_t>(f, max_eid + 1);

    // Record count of destroyed entities
    int32_t last_seen_eid = -1;
    uint32_t destroyed_ct = 0;
    for(auto eid : all_entities()) {
        int32_t cur_eid = static_cast<int32_t>(eid);

        for(int32_t i = last_seen_eid + 1; i < cur_eid; ++i) {
            ++destroyed_ct;
        }

        last_seen_eid = cur_eid;
    }

    io::serialize<uint32_t>(f, destroyed_ct);

    // Record destroyed entities
    last_seen_eid = -1;
    for(auto eid : all_entities()) {
        int32_t cur_eid = static_cast<int32_t>(eid);

        for(int32_t i = last_seen_eid + 1; i < cur_eid; ++i) {
            io::serialize<int32_t>(f, i);
        }

        last_seen_eid = cur_eid;
    }

    // Record component types
    io::serialize<uint32_t>(f, static_cast<uint32_t>(pool_containers.size()));
    for(const auto& pc : pool_containers) {
        // Record component type UID
        io::serialize<uint32_t>(f, pc.first);
        pc.second->serialize(f);
    }
}

void gorc::utility::component_system::deserialize(io::binary_input_stream& f) {
    // Clear ECS:
    for(auto entity : all_entities()) {
        erase_entity(entity);
    }

    // Parse header
    fourcc expected_fourcc("BECS");
    uint32_t expected_version = 1;

    fourcc found_fourcc(io::deserialization_constructor, f);
    uint32_t found_version = io::deserialize<uint32_t>(f);

    if(expected_fourcc != found_fourcc || expected_version != found_version) {
        throw io::file_corrupt_exception();
    }

    // Spawn entities from file
    int32_t num_entities = io::deserialize<int32_t>(f);
    for(int32_t i = 0; i < num_entities; ++i) {
        make_entity();
    }

    // Erase deleted entities
    uint32_t num_deleted_entities = io::deserialize<uint32_t>(f);
    for(uint32_t i = 0; i < num_deleted_entities; ++i) {
        int32_t de_id = io::deserialize<int32_t>(f);

        erase_entity(entity_id(de_id));
    }

    // Parse component types
    uint32_t num_component_types = io::deserialize<uint32_t>(f);
    for(uint32_t i = 0; i < num_component_types; ++i) {
        uint32_t ct_uid = io::deserialize<uint32_t>(f);

        auto ct_it = pool_containers.find(ct_uid);
        if(ct_it == pool_containers.end()) {
            // Cannot find a pool container with this UID.
            throw io::file_corrupt_exception();
        }

        ct_it->second->deserialize(f);
    }
}

void gorc::utility::component_system::update(gorc::time time) {
    for(auto& aspect : aspects) {
        aspect->update(time);
    }
}

void gorc::utility::component_system::draw(gorc::time time) {
    for(auto& aspect : aspects) {
        aspect->draw(time);
    }
}
