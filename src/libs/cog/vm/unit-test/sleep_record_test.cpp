#include "test/test.hpp"
#include "cog/vm/sleep_record.hpp"

using namespace gorc;
using namespace gorc::cog;

begin_suite(sleep_record_test);

test_case(constructor)
{
    sleep_record sr(continuation(), 0.0);
}

end_suite(sleep_record_test);
