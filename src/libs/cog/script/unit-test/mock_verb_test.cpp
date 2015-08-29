#include "cog/script/mock_verb.hpp"
#include "test/test.hpp"

using namespace gorc;
using namespace gorc::cog;

begin_suite(mock_verb_test);

test_case(invoke_mock)
{
    mock_verb v("myverb", value_type::nothing, std::vector<value_type> {});

    stack stk;
    assert_throws_logged(v.invoke(stk));

    assert_log_message(log_level::error, "invoked mock verb 'myverb'");
    assert_log_empty();
}

end_suite(mock_verb_test);
