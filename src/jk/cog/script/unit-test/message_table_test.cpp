#include "test/test.hpp"
#include "jk/cog/script/message_table.hpp"

begin_suite(message_table_test);

test_case(get_undefined_message)
{
    gorc::cog::message_table t;

    assert_true(!t.get_offset(gorc::cog::message_type::taken).has_value());

    t.set_offset(gorc::cog::message_type::taken, 19843);

    auto off = t.get_offset(gorc::cog::message_type::taken);
    assert_eq(off, size_t(19843));
}

end_suite(message_table_test);
