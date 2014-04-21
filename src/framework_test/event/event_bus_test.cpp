#include "test/test.h"
#include "framework/event/event_bus.h"
#include "framework/events/print.h"

using namespace gorc;

begin_suite(EventBusTest);

test_case(Instantiation) {
    event::event_bus eventBus;
}

test_case(fire_event) {
    event::event_bus eventBus;

    size_t call_ct = 0UL;

    assert_eq(call_ct, 0UL);

    events::print e("Test Message");

    eventBus.fire_event(e);

    assert_eq(call_ct, 0UL);

    eventBus.add_handler<events::print>([&call_ct](events::print&) {
        ++call_ct;
    });

    eventBus.fire_event(e);

    assert_eq(call_ct, 1UL);

    eventBus.add_handler<events::print>([&call_ct](events::print&) {
        ++call_ct;
    });

    eventBus.fire_event(e);

    assert_eq(call_ct, 3UL);
}

test_case(ChildFireEvent) {
    event::event_bus parentEventBus;
    event::event_bus eventBus(&parentEventBus);

    size_t call_ct = 0UL;

    assert_eq(call_ct, 0UL);

    events::print e("Test Message");

    eventBus.fire_event(e);

    assert_eq(call_ct, 0UL);

    parentEventBus.add_handler<events::print>([&call_ct](events::print&) {
        ++call_ct;
    });

    eventBus.fire_event(e);

    assert_eq(call_ct, 1UL);

    eventBus.add_handler<events::print>([&call_ct](events::print&) {
        ++call_ct;
    });

    eventBus.fire_event(e);

    assert_eq(call_ct, 3UL);
}

test_case(ChildDetachFireEvent) {
    event::event_bus parentEventBus;
    event::event_bus* eventBus = new event::event_bus(&parentEventBus);

    size_t call_ct = 0UL;

    assert_eq(call_ct, 0UL);

    events::print e("Test Message");

    eventBus->fire_event(e);

    assert_eq(call_ct, 0UL);

    parentEventBus.add_handler<events::print>([&call_ct](events::print&) {
        ++call_ct;
    });

    eventBus->fire_event(e);

    assert_eq(call_ct, 1UL);

    eventBus->add_handler<events::print>([&call_ct](events::print&) {
        ++call_ct;
    });

    eventBus->fire_event(e);

    assert_eq(call_ct, 3UL);

    delete eventBus;

    parentEventBus.fire_event(e);

    assert_eq(call_ct, 4UL);
}

test_case(CallMemberFunction) {
    class SomeClass {
    public:
        int i;

        SomeClass() : i(0) {
            return;
        }

        void HandlePrintEvent(events::print&) {
            ++i;
        }
    } someClass;

    event::event_bus eventBus;

    events::print e("Test message");

    eventBus.fire_event(e);

    assert_eq(someClass.i, 0);

    eventBus.add_handler<events::print>(std::bind(&SomeClass::HandlePrintEvent, &someClass, std::placeholders::_1));

    eventBus.fire_event(e);

    assert_eq(someClass.i, 1);
}

end_suite(EventBusTest);
