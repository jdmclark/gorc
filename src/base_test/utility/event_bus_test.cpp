#include "test/test.h"
#include "base/utility/event_bus.h"
#include "base/utility/make_unique.h"

using namespace gorc::utility;

class print {
public:
    #include "test_print_event.uid"

    const std::string message;

    print(const std::string& message)
        : message(message) {
        return;
    }
};

begin_suite(event_bus_test);

test_case(instantiation) {
    event_bus eventBus;
}

test_case(fire_event) {
    event_bus eventBus;

    size_t call_ct = 0;

    assert_eq(call_ct, 0U);

    eventBus.fire_event(print("Test Message"));

    assert_eq(call_ct, 0U);

    eventBus.add_handler<print>([&call_ct](const print&) {
        ++call_ct;
    });

    eventBus.fire_event(print("Test Message"));

    assert_eq(call_ct, 1U);

    eventBus.add_handler<print>([&call_ct](const print&) {
        ++call_ct;
    });

    eventBus.fire_event(print("Test Message"));

    assert_eq(call_ct, 3U);
}

test_case(child_fire_event) {
    event_bus parentEventBus;
    event_bus eventBus(&parentEventBus);

    size_t call_ct = 0;

    assert_eq(call_ct, 0U);

    eventBus.fire_event(print("Test Message"));

    assert_eq(call_ct, 0U);

    parentEventBus.add_handler<print>([&call_ct](const print&) {
        ++call_ct;
    });

    eventBus.fire_event(print("Test Message"));

    assert_eq(call_ct, 1U);

    eventBus.add_handler<print>([&call_ct](const print&) {
        ++call_ct;
    });

    eventBus.fire_event(print("Test Message"));

    assert_eq(call_ct, 3U);
}

test_case(child_detach_fire_event) {
    event_bus parentEventBus;
    std::unique_ptr<event_bus> eventBus = gorc::make_unique<event_bus>(&parentEventBus);

    size_t call_ct = 0;

    assert_eq(call_ct, 0U);

    eventBus->fire_event(print("Test Message"));

    assert_eq(call_ct, 0U);

    parentEventBus.add_handler<print>([&call_ct](const print&) {
        ++call_ct;
    });

    eventBus->fire_event(print("Test Message"));

    assert_eq(call_ct, 1U);

    eventBus->add_handler<print>([&call_ct](const print&) {
        ++call_ct;
    });

    eventBus->fire_event(print("Test Message"));

    assert_eq(call_ct, 3U);

    eventBus = nullptr;

    parentEventBus.fire_event(print("Test Message"));

    assert_eq(call_ct, 4U);
}

test_case(call_member_function) {
    class some_class {
    public:
        int i;

        some_class() : i(0) {
            return;
        }

        void handle_print_event(const print&) {
            ++i;
        }
    } someClass;

    event_bus eventBus;

    eventBus.fire_event(print("Test Message"));

    assert_eq(someClass.i, 0);

    eventBus.add_handler<print>(std::bind(&some_class::handle_print_event, &someClass, std::placeholders::_1));

    eventBus.fire_event(print("Test Message"));

    assert_eq(someClass.i, 1);
}

test_case(remove_child_inside_handler) {
    event_bus eventBus;
    event_bus child1(&eventBus);
    event_bus child2(&eventBus);
    event_bus child3(&eventBus);
    event_bus child4(&eventBus);
    auto child5 = gorc::make_unique<event_bus>(&eventBus);

    int call_ct = 0;

    child1.add_handler<print>([&](const print&) {
        ++call_ct;
    });

    child2.add_handler<print>([&](const print&) {
        ++call_ct;
    });

    child3.add_handler<print>([&](const print&) {
        child5 = nullptr;
        ++call_ct;
    });

    child4.add_handler<print>([&](const print&) {
        ++call_ct;
    });

    child5->add_handler<print>([&](const print&) {
        ++call_ct;
    });

    eventBus.fire_event(print("Test message"));

    assert_eq(call_ct, 4);
}

end_suite(event_bus_test);
