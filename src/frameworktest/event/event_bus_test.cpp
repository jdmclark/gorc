#include <nullunit/nullunit.h>
#include "framework/event/event_bus.h"
#include "framework/events/print_event.h"

using namespace gorc;

BeginSuite(EventBusTest);

Case(Instantiation) {
	event::event_bus eventBus;
}

Case(fire_event) {
	event::event_bus eventBus;

	size_t call_ct = 0;

	Test_Assert_Eq(call_ct, 0);

	events::print_event e("Test Message");

	eventBus.fire_event(e);

	Test_Assert_Eq(call_ct, 0);

	eventBus.add_handler<events::print_event>([&call_ct](events::print_event& e) {
		++call_ct;
	});

	eventBus.fire_event(e);

	Test_Assert_Eq(call_ct, 1);

	eventBus.add_handler<events::print_event>([&call_ct](events::print_event& e) {
		++call_ct;
	});

	eventBus.fire_event(e);

	Test_Assert_Eq(call_ct, 3);
}

Case(ChildFireEvent) {
	event::event_bus parentEventBus;
	event::event_bus eventBus(&parentEventBus);

	size_t call_ct = 0;

	Test_Assert_Eq(call_ct, 0);

	events::print_event e("Test Message");

	eventBus.fire_event(e);

	Test_Assert_Eq(call_ct, 0);

	parentEventBus.add_handler<events::print_event>([&call_ct](events::print_event& e) {
		++call_ct;
	});

	eventBus.fire_event(e);

	Test_Assert_Eq(call_ct, 1);

	eventBus.add_handler<events::print_event>([&call_ct](events::print_event& e) {
		++call_ct;
	});

	eventBus.fire_event(e);

	Test_Assert_Eq(call_ct, 3);
}

Case(ChildDetachFireEvent) {
	event::event_bus parentEventBus;
	event::event_bus* eventBus = new event::event_bus(&parentEventBus);

	size_t call_ct = 0;

	Test_Assert_Eq(call_ct, 0);

	events::print_event e("Test Message");

	eventBus->fire_event(e);

	Test_Assert_Eq(call_ct, 0);

	parentEventBus.add_handler<events::print_event>([&call_ct](events::print_event& e) {
		++call_ct;
	});

	eventBus->fire_event(e);

	Test_Assert_Eq(call_ct, 1);

	eventBus->add_handler<events::print_event>([&call_ct](events::print_event& e) {
		++call_ct;
	});

	eventBus->fire_event(e);

	Test_Assert_Eq(call_ct, 3);

	delete eventBus;

	parentEventBus.fire_event(e);

	Test_Assert_Eq(call_ct, 4);
}

Case(CallMemberFunction) {
	class SomeClass {
	public:
		int i;

		SomeClass() : i(0) {
			return;
		}

		void HandlePrintEvent(events::print_event& e) {
			++i;
		}
	} someClass;

	event::event_bus eventBus;

	events::print_event e("Test message");

	eventBus.fire_event(e);

	Test_Assert_Eq(someClass.i, 0);

	eventBus.add_handler<events::print_event>(std::bind(&SomeClass::HandlePrintEvent, &someClass, std::placeholders::_1));

	eventBus.fire_event(e);

	Test_Assert_Eq(someClass.i, 1);
}

EndSuite(EventBusTest);
