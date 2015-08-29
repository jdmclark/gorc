#include "cog/script/verb_table.hpp"
#include "test/test.hpp"

using namespace gorc;
using namespace gorc::cog;

begin_suite(verb_table_test);

test_case(redefinition)
{
    verb_table vt;

    vt.add_verb("myverb", [] {} );

    assert_throws_logged(vt.add_verb("myverb", [](int, int) -> int { return 0; }));
    assert_log_message(log_level::error, "verb 'myverb' already registered");
    assert_log_empty();
}

test_case(not_found)
{
    verb_table vt;

    assert_throws_logged(vt.get_verb_id("myverb"));
    assert_log_message(log_level::error, "verb 'myverb' does not exist");
    assert_log_empty();
}

test_case(verb_lookup)
{
    verb_table vt;

    vt.add_verb("myverb", [] { LOG_INFO("called myverb"); });

    auto id = vt.get_verb_id("myverb");
    auto const &verb = vt.get_verb(id);

    assert_eq(verb.name, std::string("myverb"));

    assert_log_empty();

    stack s;
    verb.invoke(s);

    assert_log_message(log_level::info, "called myverb");

    assert_log_empty();
}

end_suite(verb_table_test);
