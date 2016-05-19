#include "ecs/entity_component_system.hpp"
#include "ecs/component_registry.hpp"
#include "utility/service_registry.hpp"
#include "test/test.hpp"
#include <set>
#include <tuple>

using namespace gorc;

namespace {
    class mock_health_component {
    public:
        uid(10);
        int value;

        mock_health_component(int value)
            : value(value)
        {
            return;
        }
    };

    class mock_armor_component {
    public:
        uid(20);
        int value;

        mock_armor_component(int value)
            : value(value)
        {
            return;
        }
    };

    class mock_aspect : public aspect {
    public:
        mock_aspect(entity_component_system<thing_id>&, int value)
        {
            LOG_INFO(format("called mock_aspect::mock_aspect(%d)") % value);
            return;
        }

        virtual void update(time_delta) override
        {
            LOG_INFO("called mock_aspect::update");
        }
    };

    class entity_component_system_fixture : public test::fixture {
    public:
        event_bus bus;
        component_registry<thing_id> cr;
        service_registry services;

        entity_component_system_fixture()
        {
            cr.register_component_type<mock_health_component>();
            cr.register_component_type<mock_armor_component>();
            services.add(cr);

            services.add(bus);
        }
    };
}

begin_suite_fixture(entity_component_system_test,
                    entity_component_system_fixture);

test_case(aspects_updated)
{
    entity_component_system<thing_id> ecs(services);

    assert_log_empty();
    ecs.emplace_aspect<mock_aspect>(12);
    assert_log_message(log_level::info, "called mock_aspect::mock_aspect(12)");
    assert_log_empty();

    ecs.update(std::chrono::seconds(0));
    assert_log_message(log_level::info, "called mock_aspect::update");
    assert_log_empty();
}

test_case(range)
{
    entity_component_system<thing_id> ecs(services);

    auto tid = ecs.emplace_entity();
    auto tid2 = ecs.emplace_entity();

    ecs.emplace_component<mock_health_component>(tid, 5);
    ecs.emplace_component<mock_armor_component>(tid, 12);
    ecs.emplace_component<mock_health_component>(tid2, 3);

    std::set<std::tuple<int, int>> values;
    for(auto const &comp : ecs.all_components<mock_health_component>()) {
        values.emplace(static_cast<int>(comp.first), comp.second->value);
    }

    std::set<std::tuple<int, int>> expected = {
            std::make_tuple(0, 5),
            std::make_tuple(1, 3)
        };

    assert_range_eq(values, expected);
}

test_case(destroy_entity)
{
    auto delegate = bus.add_handler<entity_destroyed<thing_id>>([&](auto const &e) {
            LOG_INFO(format("entity %d destroyed") % static_cast<int>(e.entity));
        });

    entity_component_system<thing_id> ecs(services);

    auto tid = ecs.emplace_entity();
    ecs.emplace_component<mock_health_component>(tid, 3);

    assert_log_empty();
    for(auto const &em : ecs.all_components<mock_health_component>()) {
        LOG_INFO(format("found component thing %d value %d") %
                 static_cast<int>(em.first) %
                 em.second->value);
    }

    assert_log_message(log_level::info, "found component thing 0 value 3");
    assert_log_empty();

    ecs.erase_entity(tid);
    assert_log_message(log_level::info, "entity 0 destroyed");
    assert_log_empty();

    ecs.update(time_delta());

    assert_true(ecs.all_components<mock_health_component>().empty());
}

end_suite(entity_component_system_test);
