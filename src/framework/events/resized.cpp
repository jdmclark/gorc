#include "resized.h"

static const std::string GORC_EVENTS_RESIZED_EVENT_TYPE = "gorc::events::resized";
const std::string& gorc::events::resized::get_event_type() {
	return GORC_EVENTS_RESIZED_EVENT_TYPE;
}

gorc::events::resized::resized(const box<2, int>& view_size) : view_size(view_size) {
	return;
}
