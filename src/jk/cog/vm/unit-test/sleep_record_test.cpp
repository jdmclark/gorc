#include "test/test.hpp"
#include "jk/cog/vm/sleep_record.hpp"

using namespace gorc;
using namespace gorc::cog;

begin_suite(sleep_record_test);

test_case(constructor)
{
    sleep_record sr(continuation(), 0.0s);
}

end_suite(sleep_record_test);
