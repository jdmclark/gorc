#include "test/test.h"
#include "base/utility/component_system.h"
#include "base/utility/inner_join_aspect.h"
#include "base/io/memory_file.h"
#include <sstream>
#include <algorithm>

using namespace gorc;

class ijat_health_component {
public:
    #include "test_health_component.uid"

    int health;

    ijat_health_component(int health) : health(health) {
        return;
    }

    ijat_health_component(gorc::io::deserialization_constructor_tag, gorc::io::binary_input_stream& bis) {
        health = gorc::io::deserialize<int>(bis);
    }

    void serialize(gorc::io::binary_output_stream& bis) const {
        gorc::io::serialize<int>(bis, health);
    }
};

class ijat_armor_component {
public:
    #include "test_armor_component.uid"

    int damage_reduction;

    ijat_armor_component(int damage_reduction) : damage_reduction(damage_reduction) {
        return;
    }

    ijat_armor_component(gorc::io::deserialization_constructor_tag, gorc::io::binary_input_stream& bis) {
        damage_reduction = gorc::io::deserialize<int>(bis);
    }

    void serialize(gorc::io::binary_output_stream& bis) const {
        gorc::io::serialize<int>(bis, damage_reduction);
    }
};

class ijat_explodes_when_killed_component {
public:
    #include "test_explodes_component.uid"

    bool explodes;

    ijat_explodes_when_killed_component(bool explodes) : explodes(explodes) {
        return;
    }

    ijat_explodes_when_killed_component(gorc::io::deserialization_constructor_tag, gorc::io::binary_input_stream& bis) {
        explodes = gorc::io::deserialize<bool>(bis);
    }

    void serialize(gorc::io::binary_output_stream& bis) const {
        gorc::io::serialize<bool>(bis, explodes);
    }
};

class ijat_print_event {
public:
    #include "test_print_event.uid"

    std::string message;

    ijat_print_event(const std::string& message) : message(message) {
        return;
    }
};

class health_only : public inner_join_aspect<ijat_health_component> {
public:
    event_bus& bus;

    health_only(component_system& system) : inner_join_aspect(system), bus(system.bus) {
        return;
    }

    virtual void update(gorc::time, entity_id id, ijat_health_component& health) {
        std::stringstream ss;
        ss << "health_only::update(" << static_cast<int>(id) << ", " << health.health << ")";
        bus.fire_event(ijat_print_event(ss.str()));
    }

    virtual void draw(gorc::time, entity_id id, ijat_health_component& health) {
        std::stringstream ss;
        ss << "health_only::draw(" << static_cast<int>(id) << ", " << health.health << ")";
        bus.fire_event(ijat_print_event(ss.str()));
    }
};

class everything : public inner_join_aspect<ijat_health_component, ijat_armor_component, ijat_explodes_when_killed_component> {
public:
    event_bus& bus;

    everything(component_system& system) : inner_join_aspect(system), bus(system.bus) {
        return;
    }

    virtual void update(gorc::time, entity_id id, ijat_health_component& health, ijat_armor_component& armor, ijat_explodes_when_killed_component& explodes) {
        std::stringstream ss;
        ss << "everything::update("
           << static_cast<int>(id) << ", "
           << health.health << ", "
           << armor.damage_reduction << ", "
           << std::boolalpha << explodes.explodes << ")";
        bus.fire_event(ijat_print_event(ss.str()));
    }
};

class ijat_fixture : public test::fixture {
private:
    size_t i = 0;
    std::vector<std::string> results;
public:
    component_system cs;

    ijat_fixture() {
        cs.bus.add_handler<ijat_print_event>([this](const ijat_print_event& e) { results.push_back(e.message); });
    }

    std::string next_result() {
        assert_lt(i, results.size());
        return results[i++];
    }

    size_t get_pending_result_count() {
        assert_le(i, results.size());
        return results.size() - i;
    }
};

begin_suite_fixture(inner_join_aspect_test, ijat_fixture);

test_case(basic) {
    assert_eq(get_pending_result_count(), 0U);

    cs.emplace_aspect<health_only>();
    cs.update(gorc::time(timestamp(0UL), timestamp(0UL)));

    assert_eq(get_pending_result_count(), 0U);

    auto id = cs.make_entity();
    cs.update(gorc::time(timestamp(0UL), timestamp(0UL)));

    assert_eq(get_pending_result_count(), 0U);

    cs.emplace_component<ijat_armor_component>(id, 5);
    cs.update(gorc::time(timestamp(0UL), timestamp(0UL)));

    assert_eq(get_pending_result_count(), 0U);

    cs.emplace_component<ijat_health_component>(id, 16);
    cs.update(gorc::time(timestamp(0UL), timestamp(0UL)));

    assert_eq(next_result(), "health_only::update(0, 16)");

    assert_eq(get_pending_result_count(), 0U);
}

test_case(triple) {
    cs.emplace_aspect<everything>();

    auto id = cs.make_entity();
    cs.emplace_component<ijat_health_component>(id, 3);
    cs.emplace_component<ijat_armor_component>(id, 7);
    cs.emplace_component<ijat_explodes_when_killed_component>(id, true);

    cs.update(gorc::time(timestamp(0UL), timestamp(0UL)));

    assert_eq(next_result(), "everything::update(0, 3, 7, true)");

    assert_eq(get_pending_result_count(), 0U);
}

test_case(missing_one) {
    cs.emplace_aspect<everything>();

    auto missing_health = cs.make_entity();
    cs.emplace_component<ijat_armor_component>(missing_health, 0);
    cs.emplace_component<ijat_explodes_when_killed_component>(missing_health, false);

    auto missing_armor = cs.make_entity();
    cs.emplace_component<ijat_health_component>(missing_armor, 0);
    cs.emplace_component<ijat_explodes_when_killed_component>(missing_armor, true);

    auto missing_explodes = cs.make_entity();
    cs.emplace_component<ijat_health_component>(missing_explodes, 0);
    cs.emplace_component<ijat_armor_component>(missing_explodes, 0);

    cs.update(gorc::time(timestamp(0UL), timestamp(0UL)));

    assert_eq(get_pending_result_count(), 0U);

    cs.emplace_component<ijat_health_component>(missing_health, 0);

    cs.update(gorc::time(timestamp(0UL), timestamp(0UL)));

    assert_eq(next_result(), "everything::update(0, 0, 0, false)");
    assert_eq(get_pending_result_count(), 0U);
}

test_case(permutation) {
    cs.emplace_aspect<everything>();

    auto obj = cs.make_entity();
    cs.emplace_component<ijat_health_component>(obj, 16);
    cs.emplace_component<ijat_health_component>(obj, 8);
    cs.emplace_component<ijat_armor_component>(obj, 7);
    cs.emplace_component<ijat_explodes_when_killed_component>(obj, true);
    cs.emplace_component<ijat_explodes_when_killed_component>(obj, false);

    cs.update(gorc::time(timestamp(0UL), timestamp(0UL)));

    assert_eq(next_result(), "everything::update(0, 8, 7, false)");
    assert_eq(next_result(), "everything::update(0, 8, 7, true)");
    assert_eq(next_result(), "everything::update(0, 16, 7, false)");
    assert_eq(next_result(), "everything::update(0, 16, 7, true)");

    assert_eq(get_pending_result_count(), 0U);
}

test_case(multiple_entities) {
    cs.emplace_aspect<everything>();
    cs.emplace_aspect<health_only>();

    auto obj1 = cs.make_entity();
    cs.emplace_component<ijat_health_component>(obj1, 5);
    cs.emplace_component<ijat_armor_component>(obj1, 3);
    cs.emplace_component<ijat_explodes_when_killed_component>(obj1, true);

    auto obj2 = cs.make_entity();
    cs.emplace_component<ijat_health_component>(obj2, 6);
    cs.emplace_component<ijat_armor_component>(obj2, 3);

    auto obj3 = cs.make_entity();
    cs.emplace_component<ijat_explodes_when_killed_component>(obj3, false);
    cs.emplace_component<ijat_armor_component>(obj3, 76);
    cs.emplace_component<ijat_health_component>(obj3, 2);

    cs.update(gorc::time(timestamp(0UL), timestamp(0UL)));

    std::vector<std::string> results;
    for(int i = 0; i < 5; ++i) {
        results.push_back(next_result());
    }

    assert_eq(get_pending_result_count(), 0U);

    std::sort(results.begin(), results.end());

    std::vector<std::string> expected_result {
        "everything::update(0, 5, 3, true)",
        "everything::update(2, 2, 76, false)",
        "health_only::update(0, 5)",
        "health_only::update(1, 6)",
        "health_only::update(2, 2)"
    };

    assert_eq(results, expected_result);
}

test_case(ija_automatically_registers) {
    component_system cs;

    for(int i = 0; i < 12; ++i) {
        auto id = cs.make_entity();

        cs.emplace_component<ijat_health_component>(id, i);
        cs.emplace_component<ijat_armor_component>(id, i + 12);
    }

    cs.erase_entity(entity_id(4));
    cs.erase_entity(entity_id(7));

    gorc::io::memory_file f;
    auto bos = gorc::io::make_binary_output_stream(f);

    gorc::io::serialize(bos, cs);

    f.set_position(0);
    auto bis = gorc::io::make_binary_input_stream(f);

    component_system d_cs;
    d_cs.emplace_aspect<everything>();

    d_cs.deserialize(bis);

    int ct = 0;
    for(auto id : d_cs.all_entities()) {
        assert_ne(static_cast<int>(id), 4);
        assert_ne(static_cast<int>(id), 7);
        ++ct;
    }

    assert_eq(ct, 10);

    ct = 0;
    for(auto& hc : d_cs.all_components<ijat_health_component>()) {
        assert_ne(static_cast<int>(hc.first), 4);
        assert_ne(static_cast<int>(hc.first), 7);
        assert_eq(static_cast<int>(hc.first), hc.second.health);
        ++ct;
    }

    assert_eq(ct, 10);

    ct = 0;
    for(auto& hc : d_cs.all_components<ijat_armor_component>()) {
        assert_ne(static_cast<int>(hc.first), 4);
        assert_ne(static_cast<int>(hc.first), 7);
        assert_eq(static_cast<int>(hc.first) + 12, hc.second.damage_reduction);
        ++ct;
    }

    assert_eq(ct, 10);
}

end_suite(inner_join_aspect_test);
