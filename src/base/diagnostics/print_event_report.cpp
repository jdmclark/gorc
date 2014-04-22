#include "print_event_report.h"
#include "error_level.h"
#include "base/events/print.h"
#include "base/utility/event_bus.h"

using namespace gorc::diagnostics;

print_event_report::print_event_report(event_bus& eventBus)
    : EventBus(eventBus), errorCount(0), warningCount(0) {
    return;
}

void print_event_report::add_error(const std::string& stage, const std::string& reason, const error_location& errorLocation) {
    error e(error_level::error, stage, reason, errorLocation);
    events::print evt(e);
    EventBus.fire_event(evt);
    ++errorCount;
}

void print_event_report::add_warning(const std::string& stage, const std::string& reason, const error_location& errorLocation) {
    error e(error_level::warning, stage, reason, errorLocation);
    events::print evt(e);
    EventBus.fire_event(evt);
    ++warningCount;
}

void print_event_report::add_critical_error(const std::string& stage, const std::string& reason) {
    error e(error_level::critical_error, stage, reason, error_location());
    events::print evt(e);
    EventBus.fire_event(evt);
    ++errorCount;
}

unsigned int print_event_report::get_error_count() const {
    return errorCount;
}

unsigned int print_event_report::get_warning_count() const {
    return warningCount;
}
