#include "test/test.hpp"
#include "jk/vfs/episode_entry_type.hpp"

begin_suite(episode_entry_type_test);

test_case(invalid)
{
    auto t = static_cast<gorc::episode_entry_type>(-1);
    assert_throws(gorc::as_string(t),
                  std::range_error,
                  "not a valid episode_entry_type");
}

end_suite(episode_entry_type_test);
