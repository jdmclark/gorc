#include "test/test.hpp"
#include "jk/cog/vm/virtual_machine.hpp"
#include "jk/cog/vm/executor.hpp"

using namespace gorc;
using namespace gorc::cog;

begin_suite(virtual_machine_test);

test_case(empty_continuation)
{
    verb_table verbs;
    service_registry services;
    executor exec(verbs);
    virtual_machine vm;

    continuation empty_continuation;

    vm.execute(verbs, exec, services, empty_continuation);
}

end_suite(virtual_machine_test);
