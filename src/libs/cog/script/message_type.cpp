#include "message_type.hpp"

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
