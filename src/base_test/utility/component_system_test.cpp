#include "test/test.h"
#include "base/utility/component_system.h"
#include "base/io/memory_file.h"
#include "base/io/binary_input_stream.h"
#include "base/io/binary_output_stream.h"
#include "base/io/exception.h"
#include "base/utility/fourcc.h"
#include <queue>

using namespace gorc;

class health_component {
public:
    #include "test_health_component.uid"

    int health;

    health_component(int health) : health(health) {
        return;
    }

    health_component(gorc::io::deserialization_constructor_tag, gorc::io::binary_input_stream& f) {
        health = gorc::io::deserialize<int>(f);
    }

    void serialize(gorc::io::binary_output_stream& f) const {
        gorc::io::serialize(f, health);
    }
};

class armor_component {
public:
    #include "test_armor_component.uid"

    int damage_reduction;

    armor_component(int damage_reduction) : damage_reduction(damage_reduction) {
        return;
    }

    armor_component(gorc::io::deserialization_constructor_tag, gorc::io::binary_input_stream& f) {
        damage_reduction = gorc::io::deserialize<int>(f);
    }

    void serialize(gorc::io::binary_output_stream& f) const {
        gorc::io::serialize(f, damage_reduction);
    }
};

class test_print_event {
public:
    #include "test_print_event.uid"

    int v;

    test_print_event(int v) : v(v) {
        return;
    }
};

class test_aspect : public aspect {
public:
    event_bus& e;

    test_aspect(component_system& parent_system, int arg1, int arg2, int arg3, bool& ctor_was_called) : e(parent_system.bus) {
        assert_eq(arg1, 5);
        assert_eq(arg2, 7);
        assert_eq(arg3, 11);

        ctor_was_called = true;
    }

    virtual void update(gorc::time) override {
        e.fire_event(test_print_event(857));
    }

    virtual void draw(gorc::time) override {
        e.fire_event(test_print_event(555));
    }
};

begin_suite(component_system_test);

test_case(create_entity) {
    component_system cs;

    auto id = cs.make_entity();

    assert_eq(static_cast<int>(id), 0);
}

test_case(destroy_entity) {
    component_system cs;

    auto id = cs.make_entity();

    cs.erase_entity(id);
}

test_case(create_component) {
    component_system cs;

    auto id = cs.make_entity();

    cs.emplace_component<health_component>(id, 100);

    int ct = 0;
    for(auto& cmp : cs.find_component<health_component>(id)) {
        ++ct;
        assert_eq(cmp.second.health, 100);
    }

    assert_eq(ct, 1);
}

test_case(destroy_component) {
    component_system cs;

    auto id = cs.make_entity();

    for(int i = 0; i < 5; ++i) {
        cs.emplace_component<health_component>(id, 100);
    }

    int ct = 0;
    for(auto& cmp : cs.find_component<health_component>(id)) {
        ++ct;
    }

    assert_eq(ct, 5);

    ct = 0;
    auto find_pair = cs.find_component<health_component>(id);
    for(auto it = find_pair.begin(); it != find_pair.end();) {
        if(ct < 3) {
            auto cur_it = it;
            ++it;
            cs.erase_component(cur_it);
        }
        else {
            ++it;
        }

        ++ct;
    }

    assert_eq(ct, 5);

    ct = 0;
    for(auto& cmp : cs.find_component<health_component>(id)) {
        ++ct;
    }

    assert_eq(ct, 2);
}

test_case(destroy_entity_destroys_components) {
    component_system cs;

    auto id = cs.make_entity();

    cs.emplace_component<health_component>(id, 100);

    for(int i = 0; i < 5; ++i) {
        cs.emplace_component<armor_component>(id, 6);
    }

    int ct = 0;
    for(auto& cmp : cs.find_component<armor_component>(id)) {
        ++ct;
    }

    assert_eq(ct, 5);

    for(auto& cmp : cs.find_component<health_component>(id)) {
        ++ct;
    }

    assert_eq(ct, 6);

    cs.erase_entity(id);

    for(auto& cmp : cs.find_component<armor_component>(id)) {
        assert_always("found an armor component");
    }

    for(auto& cmp : cs.find_component<health_component>(id)) {
        assert_always("found a health component");
    }
}

test_case(has_event_bus) {
    component_system cs;

    int tpe_result = 0;
    int tpe_check = 0;
    cs.bus.add_handler<test_print_event>([&](test_print_event& e) {
        tpe_result = e.v;
        tpe_check = 364;
    });

    cs.bus.fire_event(test_print_event(8574));

    assert_eq(tpe_result, 8574);
    assert_eq(tpe_check, 364);
}

test_case(emplace_aspect) {
    component_system cs;

    int update_was_called = 0;
    cs.bus.add_handler<test_print_event>([&](test_print_event& e) {
        update_was_called = e.v;
    });

    assert_eq(0, update_was_called);

    bool ctor_was_called = false;
    cs.emplace_aspect<test_aspect>(5, 7, 11, ctor_was_called);

    assert_eq(0, update_was_called);

    cs.update(gorc::time(timestamp(0UL), timestamp(0UL)));
    assert_eq(857, update_was_called);

    cs.draw(gorc::time(timestamp(0UL), timestamp(0UL)));
    assert_eq(555, update_was_called);
}

test_case(enumerate_entities) {
    component_system cs;

    for(int i = 0; i < 6; ++i) {
        cs.make_entity();
    }

    auto id = cs.make_entity();

    for(int i = 0; i < 12; ++i) {
        cs.make_entity();
    }

    cs.erase_entity(id);

    for(auto entity : cs.all_entities()) {
        expect_ne(entity, id);
    }
}

test_case(enumerate_components) {
    component_system cs;

    auto id = cs.make_entity();

    for(int i = 0; i < 12; ++i) {
        cs.emplace_component<health_component>(id, i);
    }

    std::priority_queue<int> seen_health;
    for(auto& cmp : cs.all_components<health_component>()) {
        assert_eq(cmp.first, id);
        seen_health.push(cmp.second.health);
    }

    for(int i = 11; i >= 0; --i) {
        assert_eq(seen_health.top(), i);
        seen_health.pop();
    }

    assert_true(seen_health.empty());
}

test_case(serialize) {
    component_system cs;

    component_serializer c_serializer;
    c_serializer.add_default_serializer<health_component>();
    c_serializer.add_serializer<armor_component>([](armor_component const &c,
                                                    io::binary_output_stream &os) {
        c.serialize(os);
    });

    component_deserializer c_deserializer;
    c_deserializer.add_deserializer<health_component>([](component_system &cs,
                                                         entity_id id,
                                                         io::binary_input_stream &is) {
        cs.emplace_component<health_component>(id,
                                               io::deserialization_constructor,
                                               is);
    });
    c_deserializer.add_default_deserializer<armor_component>();


    for(int i = 0; i < 12; ++i) {
        auto id = cs.make_entity();

        cs.emplace_component<health_component>(id, i);
        cs.emplace_component<armor_component>(id, i + 12);
    }

    cs.erase_entity(entity_id(4));
    cs.erase_entity(entity_id(7));

    gorc::io::memory_file f;
    auto bos = gorc::io::make_binary_output_stream(f);

    cs.serialize(bos, c_serializer);

    f.set_position(0);
    auto bis = gorc::io::make_binary_input_stream(f);

    component_system d_cs;
    d_cs.register_component_type<health_component>();
    d_cs.register_component_type<armor_component>();

    d_cs.deserialize(bis, c_deserializer);

    int ct = 0;
    for(auto id : d_cs.all_entities()) {
        assert_ne(static_cast<int>(id), 4);
        assert_ne(static_cast<int>(id), 7);
        ++ct;
    }

    assert_eq(ct, 10);

    ct = 0;
    for(auto& hc : d_cs.all_components<health_component>()) {
        assert_ne(static_cast<int>(hc.first), 4);
        assert_ne(static_cast<int>(hc.first), 7);
        assert_eq(static_cast<int>(hc.first), hc.second.health);
        ++ct;
    }

    assert_eq(ct, 10);

    ct = 0;
    for(auto& hc : d_cs.all_components<armor_component>()) {
        assert_ne(static_cast<int>(hc.first), 4);
        assert_ne(static_cast<int>(hc.first), 7);
        assert_eq(static_cast<int>(hc.first) + 12, hc.second.damage_reduction);
        ++ct;
    }

    assert_eq(ct, 10);
}

test_case(deserialize_without_register) {
    component_system cs;

    for(int i = 0; i < 12; ++i) {
        auto id = cs.make_entity();

        cs.emplace_component<health_component>(id, i);
        cs.emplace_component<armor_component>(id, i + 12);
    }

    cs.erase_entity(entity_id(4));
    cs.erase_entity(entity_id(7));

    gorc::io::memory_file f;
    auto bos = gorc::io::make_binary_output_stream(f);

    component_serializer cs_s;
    cs_s.add_default_serializer<health_component>();
    cs_s.add_default_serializer<armor_component>();

    cs.serialize(bos, cs_s);

    f.set_position(0);
    auto bis = gorc::io::make_binary_input_stream(f);

    component_system d_cs;

    component_deserializer cs_d;
    cs_d.add_default_deserializer<health_component>();
    cs_d.add_default_deserializer<armor_component>();

    try {
        d_cs.deserialize(bis, cs_d);
        assert_always("did not throw exception");
    }
    catch(const gorc::io::file_corrupt_exception&) { }
}

test_case(deserialize_bad_fourcc) {
    component_system cs;

    gorc::io::memory_file f;
    auto bos = gorc::io::make_binary_output_stream(f);

    gorc::io::serialize(bos, gorc::fourcc("BADF"));
    gorc::io::serialize<uint32_t>(bos, 1);

    auto bis = gorc::io::make_binary_input_stream(f);
    f.set_position(0);

    component_deserializer cs_d;

    try {
        cs.deserialize(bis, cs_d);
        assert_always("did not throw exception");
    }
    catch(const gorc::io::file_corrupt_exception&) { }
}

test_case(deserialize_bad_version) {
    component_system cs;

    gorc::io::memory_file f;
    auto bos = gorc::io::make_binary_output_stream(f);

    gorc::io::serialize(bos, gorc::fourcc("BECS"));
    gorc::io::serialize<uint32_t>(bos, 0);

    auto bis = gorc::io::make_binary_input_stream(f);
    f.set_position(0);

    component_deserializer cs_d;

    try {
        cs.deserialize(bis, cs_d);
        assert_always("did not throw exception");
    }
    catch(const gorc::io::file_corrupt_exception&) { }
}

test_case(deserialize_clears) {
    component_system cs;

    component_serializer cs_s;
    cs_s.add_default_serializer<health_component>();
    cs_s.add_default_serializer<armor_component>();

    component_deserializer cs_d;
    cs_d.add_default_deserializer<health_component>();
    cs_d.add_default_deserializer<armor_component>();

    cs.make_entity();
    auto eid = cs.make_entity();
    cs.emplace_component<health_component>(eid, 10);
    cs.emplace_component<health_component>(eid, 10);
    cs.emplace_component<health_component>(eid, 10);

    gorc::io::memory_file f;
    auto bos = gorc::io::make_binary_output_stream(f);

    cs.serialize(bos, cs_s);

    f.set_position(0);

    component_system ds;

    ds.register_component_type<health_component>();

    ds.make_entity();
    ds.make_entity();
    ds.make_entity();
    ds.make_entity();
    auto fid = ds.make_entity();
    ds.emplace_component<armor_component>(fid, 5);
    ds.emplace_component<armor_component>(fid, 5);

    int ct = 0;
    for(const auto& entity : ds.all_entities()) {
        ++ct;
    }

    assert_eq(ct, 5);

    ct = 0;
    for(const auto& ac : ds.all_components<armor_component>()) {
        assert_eq(ac.first, fid);
        assert_eq(ac.second.damage_reduction, 5);
        ++ct;
    }

    assert_eq(ct, 2);

    for(const auto& ac : ds.all_components<health_component>()) {
        assert_always("found health component, shouldn't have");
    }

    auto bis = gorc::io::make_binary_input_stream(f);
    ds.deserialize(bis, cs_d);

    ct = 0;
    for(const auto& ac : ds.all_components<health_component>()) {
        assert_eq(ac.first, eid);
        assert_eq(ac.second.health, 10);
        ++ct;
    }

    assert_eq(ct, 3);

    ct = 0;
    for(const auto& entity : ds.all_entities()) {
        ++ct;
    }

    assert_eq(ct, 2);

    for(const auto& ac : ds.all_components<armor_component>()) {
        assert_always("found armor component, shouldn't have");
    }
}

end_suite(component_system_test);
