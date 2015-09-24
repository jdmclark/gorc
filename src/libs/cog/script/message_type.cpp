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
}

char const* gorc::cog::as_string(message_type mt)
{
    auto it = mt_map.find(mt);
    if(it != mt_map.end()) {
        return it->second;
    }

    throw std::range_error("not a valid message_type");
}

void gorc::cog::add_messages_to_constant_table(constant_table &table)
{
    table.emplace("global0", message_id(static_cast<int>(message_type::global0)));
    table.emplace("activate", message_id(static_cast<int>(message_type::activated)));
    table.emplace("activated", message_id(static_cast<int>(message_type::activated)));
    table.emplace("removed", message_id(static_cast<int>(message_type::removed)));
    table.emplace("startup", message_id(static_cast<int>(message_type::startup)));
    table.emplace("timer", message_id(static_cast<int>(message_type::timer)));
    table.emplace("blocked", message_id(static_cast<int>(message_type::blocked)));
    table.emplace("entered", message_id(static_cast<int>(message_type::entered)));
    table.emplace("exited", message_id(static_cast<int>(message_type::exited)));
    table.emplace("crossed", message_id(static_cast<int>(message_type::sighted)));
    table.emplace("damaged", message_id(static_cast<int>(message_type::damaged)));
    table.emplace("arrived", message_id(static_cast<int>(message_type::arrived)));
    table.emplace("killed", message_id(static_cast<int>(message_type::killed)));
    table.emplace("pulse", message_id(static_cast<int>(message_type::pulse)));
    table.emplace("touched", message_id(static_cast<int>(message_type::touched)));
    table.emplace("created", message_id(static_cast<int>(message_type::created)));
    table.emplace("loading", message_id(static_cast<int>(message_type::loading)));
    table.emplace("selected", message_id(static_cast<int>(message_type::selected)));
    table.emplace("deselected", message_id(static_cast<int>(message_type::deselected)));
    table.emplace("autoselect", message_id(static_cast<int>(message_type::autoselect)));
    table.emplace("changed", message_id(static_cast<int>(message_type::changed)));
    table.emplace("deactivated", message_id(static_cast<int>(message_type::deactivated)));
    table.emplace("shutdown", message_id(static_cast<int>(message_type::shutdown)));
    table.emplace("respawn", message_id(static_cast<int>(message_type::respawn)));
    table.emplace("ai_event", message_id(static_cast<int>(message_type::ai_event)));
    table.emplace("skill", message_id(static_cast<int>(message_type::skill)));
    table.emplace("taken", message_id(static_cast<int>(message_type::taken)));
    table.emplace("user0", message_id(static_cast<int>(message_type::user0)));
    table.emplace("user1", message_id(static_cast<int>(message_type::user1)));
    table.emplace("user2", message_id(static_cast<int>(message_type::user2)));
    table.emplace("user3", message_id(static_cast<int>(message_type::user3)));
    table.emplace("user4", message_id(static_cast<int>(message_type::user4)));
    table.emplace("user5", message_id(static_cast<int>(message_type::user5)));
    table.emplace("user6", message_id(static_cast<int>(message_type::user6)));
    table.emplace("user7", message_id(static_cast<int>(message_type::user7)));
    table.emplace("new_player", message_id(static_cast<int>(message_type::new_player)));
    table.emplace("fire", message_id(static_cast<int>(message_type::fire)));
    table.emplace("join", message_id(static_cast<int>(message_type::join)));
    table.emplace("leave", message_id(static_cast<int>(message_type::leave)));
    table.emplace("splash", message_id(static_cast<int>(message_type::splash)));
    table.emplace("trigger", message_id(static_cast<int>(message_type::trigger)));
}
