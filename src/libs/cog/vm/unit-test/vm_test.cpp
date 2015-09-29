#include "test/test.hpp"
#include "cog/vm/vm.hpp"

using namespace gorc;
using namespace gorc::cog;

begin_suite(vm_test);

test_case(empty_continuation)
{
    verb_table verbs;
    service_registry services;

    vm virtual_machine;

    continuation empty_continuation;

    virtual_machine.execute(verbs, services, empty_continuation);
}

end_suite(vm_test);
