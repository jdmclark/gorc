#include "message_type.hpp"
#include <unordered_map>
#include "utility/enum_hash.hpp"
#include "log/log.hpp"

using namespace gorc;
using namespace cog;

namespace {
    std::unordered_map<message_type, char const*, enum_hash<message_type>> mt_map {
        { message_type::global0, "global0" },
        { message_type::activated, "activated" },
        { message_type::removed, "removed" },
        { message_type::startup, "startup" },
        { message_type::timer, "timer" },
        { message_type::blocked, "blocked" },
        { message_type::entered, "entered" },
        { message_type::exited, "exited" },
        { message_type::crossed, "crossed" },
        { message_type::sighted, "sighted" },
        { message_type::damaged, "damaged" },
        { message_type::arrived, "arrived" },
        { message_type::killed, "killed" },
        { message_type::pulse, "pulse" },
        { message_type::touched, "touched" },
        { message_type::created, "created" },
        { message_type::loading, "loading" },
        { message_type::selected, "selected" },
        { message_type::deselected, "deselected" },
        { message_type::autoselect, "autoselect" },
        { message_type::changed, "changed" },
        { message_type::deactivated, "deactivated" },
        { message_type::shutdown, "shutdown" },
        { message_type::respawn, "respawn" },
        { message_type::ai_event, "ai_event" },
        { message_type::skill, "skill" },
        { message_type::taken, "taken" },
        { message_type::user0, "user0" },
        { message_type::user1, "user1" },
        { message_type::user2, "user2" },
        { message_type::user3, "user3" },
        { message_type::user4, "user4" },
        { message_type::user5, "user5" },
        { message_type::user6, "user6" },
        { message_type::user7, "user7" },
        { message_type::new_player, "new_player" },
        { message_type::fire, "fire" },
        { message_type::join, "join" },
        { message_type::leave, "leave" },
        { message_type::splash, "splash" },
        { message_type::trigger, "trigger" }
    };

    std::unordered_map<std::string, message_type> str_map {
        { "global0", message_type::global0 },
        { "activate", message_type::activated },
        { "activated", message_type::activated },
        { "removed", message_type::removed },
        { "startup", message_type::startup },
        { "timer", message_type::timer },
        { "blocked", message_type::blocked },
        { "entered", message_type::entered },
        { "exited", message_type::exited },
        { "crossed", message_type::sighted },
        { "damaged", message_type::damaged },
        { "arrived", message_type::arrived },
        { "killed", message_type::killed },
        { "pulse", message_type::pulse },
        { "touched", message_type::touched },
        { "created", message_type::created },
        { "loading", message_type::loading },
        { "selected", message_type::selected },
        { "deselected", message_type::deselected },
        { "autoselect", message_type::autoselect },
        { "changed", message_type::changed },
        { "deactivated", message_type::deactivated },
        { "shutdown", message_type::shutdown },
        { "respawn", message_type::respawn },
        { "ai_event", message_type::ai_event },
        { "skill", message_type::skill },
        { "taken", message_type::taken },
        { "user0", message_type::user0 },
        { "user1", message_type::user1 },
        { "user2", message_type::user2 },
        { "user3", message_type::user3 },
        { "user4", message_type::user4 },
        { "user5", message_type::user5 },
        { "user6", message_type::user6 },
        { "user7", message_type::user7 },
        { "new_player", message_type::new_player },
        { "fire", message_type::fire },
        { "join", message_type::join },
        { "leave", message_type::leave },
        { "splash", message_type::splash },
        { "trigger", message_type::trigger }
    };
}

char const* gorc::cog::as_string(message_type mt)
{
    auto it = mt_map.find(mt);
    if(it != mt_map.end()) {
        return it->second;
    }

    throw std::range_error("not a valid message_type");
}

message_type gorc::cog::as_message_type(std::string const &str)
{
    auto it = str_map.find(str);
    if(it == str_map.end()) {
        LOG_FATAL(format("unknown message type \'%s\'") % str);
    }

    return it->second;
}

void gorc::cog::add_messages_to_constant_table(constant_table &table)
{
    for(auto const &em : str_map) {
        table.emplace(em.first, message_id(static_cast<int>(em.second)));
    }
}
