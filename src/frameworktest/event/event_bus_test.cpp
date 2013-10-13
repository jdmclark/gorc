#include <nullunit/nullunit.h>
#include "framework/event/event_bus.h"
#include "framework/events/print_event.h"

using namespace Gorc;

BeginSuite(EventBusTest);

Case(Instantiation) {
	Event::EventBus eventBus;
}

Case(FireEvent) {
	Event::EventBus eventBus;

	size_t call_ct = 0;

	Test_Assert_Eq(call_ct, 0);

	Events::PrintEvent e("Test Message");

	eventBus.FireEvent(e);

	Test_Assert_Eq(call_ct, 0);

	eventBus.AddHandler<Events::PrintEvent>([&call_ct](Events::PrintEvent& e) {
		++call_ct;
	});

	eventBus.FireEvent(e);

	Test_Assert_Eq(call_ct, 1);

	eventBus.AddHandler<Events::PrintEvent>([&call_ct](Events::PrintEvent& e) {
		++call_ct;
	});

	eventBus.FireEvent(e);

	Test_Assert_Eq(call_ct, 3);
}

Case(ChildFireEvent) {
	Event::EventBus parentEventBus;
	Event::EventBus eventBus(&parentEventBus);

	size_t call_ct = 0;

	Test_Assert_Eq(call_ct, 0);

	Events::PrintEvent e("Test Message");

	eventBus.FireEvent(e);

	Test_Assert_Eq(call_ct, 0);

	parentEventBus.AddHandler<Events::PrintEvent>([&call_ct](Events::PrintEvent& e) {
		++call_ct;
	});

	eventBus.FireEvent(e);

	Test_Assert_Eq(call_ct, 1);

	eventBus.AddHandler<Events::PrintEvent>([&call_ct](Events::PrintEvent& e) {
		++call_ct;
	});

	eventBus.FireEvent(e);

	Test_Assert_Eq(call_ct, 3);
}

Case(ChildDetachFireEvent) {
	Event::EventBus parentEventBus;
	Event::EventBus* eventBus = new Event::EventBus(&parentEventBus);

	size_t call_ct = 0;

	Test_Assert_Eq(call_ct, 0);

	Events::PrintEvent e("Test Message");

	eventBus->FireEvent(e);

	Test_Assert_Eq(call_ct, 0);

	parentEventBus.AddHandler<Events::PrintEvent>([&call_ct](Events::PrintEvent& e) {
		++call_ct;
	});

	eventBus->FireEvent(e);

	Test_Assert_Eq(call_ct, 1);

	eventBus->AddHandler<Events::PrintEvent>([&call_ct](Events::PrintEvent& e) {
		++call_ct;
	});

	eventBus->FireEvent(e);

	Test_Assert_Eq(call_ct, 3);

	delete eventBus;

	parentEventBus.FireEvent(e);

	Test_Assert_Eq(call_ct, 4);
}

Case(CallMemberFunction) {
	class SomeClass {
	public:
		int i;

		SomeClass() : i(0) {
			return;
		}

		void HandlePrintEvent(Events::PrintEvent& e) {
			++i;
		}
	} someClass;

	Event::EventBus eventBus;

	Events::PrintEvent e("Test message");

	eventBus.FireEvent(e);

	Test_Assert_Eq(someClass.i, 0);

	eventBus.AddHandler<Events::PrintEvent>(std::bind(&SomeClass::HandlePrintEvent, &someClass, std::placeholders::_1));

	eventBus.FireEvent(e);

	Test_Assert_Eq(someClass.i, 1);
}

EndSuite(EventBusTest);
