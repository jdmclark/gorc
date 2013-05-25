#include "printeventreport.h"
#include "errorlevel.h"
#include "framework/events/printevent.h"
#include "framework/event/eventbus.h"

using namespace Gorc::Diagnostics;

PrintEventReport::PrintEventReport(Event::EventBus& eventBus)
	: EventBus(eventBus), errorCount(0), warningCount(0) {
	return;
}

void PrintEventReport::AddError(const std::string& stage, const std::string& reason, const ErrorLocation& errorLocation) {
	Error e(ErrorLevel::Error, stage, reason, errorLocation);
	Events::PrintEvent evt(e);
	EventBus.FireEvent(evt);
	++errorCount;
}

void PrintEventReport::AddWarning(const std::string& stage, const std::string& reason, const ErrorLocation& errorLocation) {
	Error e(ErrorLevel::Warning, stage, reason, errorLocation);
	Events::PrintEvent evt(e);
	EventBus.FireEvent(evt);
	++warningCount;
}

void PrintEventReport::AddCriticalError(const std::string& stage, const std::string& reason) {
	Error e(ErrorLevel::CriticalError, stage, reason, ErrorLocation());
	Events::PrintEvent evt(e);
	EventBus.FireEvent(evt);
	++errorCount;
}

unsigned int PrintEventReport::GetErrorCount() const {
	return errorCount;
}

unsigned int PrintEventReport::GetWarningCount() const {
	return warningCount;
}
