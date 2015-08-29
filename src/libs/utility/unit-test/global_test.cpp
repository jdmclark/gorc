#include "test/test.hpp"
#include "utility/global.hpp"

namespace {
    class dep : public gorc::global {
        template <typename GlobalT> friend class gorc::global_factory;

        dep()
        {
            return;
        }

    public:
        int value = 0;
    };

    class base : public gorc::global {
        template <typename GlobalT> friend class gorc::global_factory;

        std::shared_ptr<dep> the_dep;

        base()
            : the_dep(reserve_global_dependency<dep>())
        {
            return;
        }

    public:
        void set_dep_value(int v)
        {
            the_dep->value = v;
        }
    };
}

begin_suite(global_test);

test_case(register_dependency)
{
    auto the_base = gorc::get_global<base>();
    auto the_dep = gorc::get_global<dep>();

    assert_eq(the_dep->value, 0);

    the_base->set_dep_value(18);

    assert_eq(the_dep->value, 18);
}

end_suite(global_test);
