#include "constant_table.hpp"
#include "message_type.hpp"

void gorc::cog::default_populate_constant_table(constant_table &table)
{
    add_messages_to_constant_table(table);
}
