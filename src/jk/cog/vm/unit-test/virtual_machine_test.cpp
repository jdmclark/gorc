#include "jk/cog/vm/executor.hpp"
#include "jk/cog/vm/virtual_machine.hpp"
#include "test/test.hpp"

using namespace gorc;
using namespace gorc::cog;

begin_suite(virtual_machine_test);

test_case(empty_continuation)
{
    heap globals;
    verb_table verbs;
    service_registry services;
    services.add(verbs);

    executor exec(services);
    virtual_machine vm;

    continuation empty_continuation;

    vm.execute(globals, verbs, exec, services, empty_continuation);
}

end_suite(virtual_machine_test);
