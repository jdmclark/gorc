#include "ecs/entity_component_system.hpp"
#include "test/test.hpp"
#include <set>
#include <tuple>

using namespace gorc;

namespace {
    class mock_health_component {
    public:
        int value;

        mock_health_component(int value)
            : value(value)
        {
            return;
        }
    };

    class mock_armor_component {
    public:
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
}

begin_suite(entity_component_system_test);

test_case(aspects_updated)
{
    entity_component_system<thing_id> ecs;

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
    entity_component_system<thing_id> ecs;

    auto tid = ecs.emplace_entity();
    auto tid2 = ecs.emplace_entity();

    ecs.emplace_component<mock_health_component>(tid, 5);
    ecs.emplace_component<mock_armor_component>(tid, 12);
    ecs.emplace_component<mock_health_component>(tid2, 3);

    std::set<std::tuple<int, int>> values;
    for(auto const &comp : ecs.component_range<mock_health_component>()) {
        values.emplace(static_cast<int>(comp.first), comp.second->value);
    }

    std::set<std::tuple<int, int>> expected = {
            std::make_tuple(0, 5),
            std::make_tuple(1, 3)
        };

    assert_range_eq(values, expected);
}

end_suite(entity_component_system_test);
