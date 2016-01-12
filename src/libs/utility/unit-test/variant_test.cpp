#include "test/test.hpp"
#include "utility/variant.hpp"
#include "log/log.hpp"
#include <memory>
#include <vector>

using namespace gorc;

namespace {
    class mock_value {
    public:
        mock_value() {
            LOG_INFO("mock_value default constructor");
        }

        mock_value(mock_value&&) {
            LOG_INFO("mock_value move constructor");
        }

        mock_value(mock_value const &) {
            LOG_INFO("mock_value copy constructor");
        }

        ~mock_value() {
            LOG_INFO("mock_value destructor");
        }

        mock_value& operator=(mock_value&&) {
            LOG_INFO("mock_value move assignment");
            return *this;
        }

        mock_value& operator=(mock_value const &) {
            LOG_INFO("mock_value copy assignment");
            return *this;
        }
    };

    class int_collector_visitor {
    public:
        int visit(int v) const
        {
            return v;
        }

        int visit(std::unique_ptr<int> const &v) const
        {
            return *v;
        }
    };

    class mutable_int_collector_visitor {
    public:
        int total = 0;

        void visit(int v)
        {
            total += v;
        }

        void visit(std::unique_ptr<int> const &v)
        {
            total += *v;
        }
    };

    class zero_int_visitor {
    public:
        void visit(int &v) const
        {
            v = 0;
        }

        void visit(std::unique_ptr<int> &v) const
        {
            *v = 0;
        }
    };
}

begin_suite(variant_test);

test_case(single_variant_set)
{
    variant<int> v(5);
    assert_eq(v.accept(int_collector_visitor()), 5);
}

test_case(single_variant_set_uncopyable)
{
    variant<std::unique_ptr<int>> v(std::make_unique<int>(5));
    assert_eq(v.accept(int_collector_visitor()), 5);
}

test_case(variant_assignment)
{
    variant<int, std::unique_ptr<int>> v(5);
    assert_eq(v.accept(int_collector_visitor()), 5);
    v = std::make_unique<int>(583);
    assert_eq(v.accept(int_collector_visitor()), 583);
}

test_case(variant_destructors)
{
    class mock_a {
    public:
        ~mock_a()
        {
            LOG_INFO("destructor a");
        }
    };

    class mock_b {
    public:
        ~mock_b()
        {
            LOG_INFO("destructor b");
        }
    };

    mock_a a;
    mock_b b;
    assert_log_empty();

    variant<mock_a, mock_b> v(a);
    assert_log_empty();

    v = b;
    assert_log_message(log_level::info, "destructor a");
    assert_log_empty();

    v = mock_a();
    assert_log_message(log_level::info, "destructor b");
    assert_log_message(log_level::info, "destructor a");
    assert_log_empty();
}

test_case(immutable_variant_immutable_visitor)
{
    variant<int, std::unique_ptr<int>> const a(5);
    variant<int, std::unique_ptr<int>> const b(std::make_unique<int>(7));

    assert_eq(a.accept(int_collector_visitor()), 5);
    assert_eq(b.accept(int_collector_visitor()), 7);
}

test_case(mutable_variant_immutable_visitor)
{
    variant<int, std::unique_ptr<int>> a(5);
    variant<int, std::unique_ptr<int>> b(std::make_unique<int>(7));

    assert_eq(a.accept(int_collector_visitor()), 5);
    assert_eq(b.accept(int_collector_visitor()), 7);

    a.accept(zero_int_visitor());
    assert_eq(a.accept(int_collector_visitor()), 0);

    b.accept(zero_int_visitor());
    assert_eq(b.accept(int_collector_visitor()), 0);
}

test_case(immutable_variant_mutable_visitor)
{
    variant<int, std::unique_ptr<int>> const a(5);
    variant<int, std::unique_ptr<int>> const b(std::make_unique<int>(7));

    mutable_int_collector_visitor v;
    assert_eq(v.total, 0);
    a.accept(v);
    assert_eq(v.total, 5);
    b.accept(v);
    assert_eq(v.total, 12);
}

test_case(mutable_variant_mutable_visitor)
{
    variant<int, std::unique_ptr<int>> a(5);
    variant<int, std::unique_ptr<int>> b(std::make_unique<int>(7));

    mutable_int_collector_visitor v;
    assert_eq(v.total, 0);
    a.accept(v);
    assert_eq(v.total, 5);
    b.accept(v);
    assert_eq(v.total, 12);

    a.accept(zero_int_visitor());
    b.accept(zero_int_visitor());

    mutable_int_collector_visitor w;
    assert_eq(w.total, 0);
    a.accept(w);
    assert_eq(w.total, 0);
    b.accept(w);
    assert_eq(w.total, 0);
}

test_case(rvalue_copy)
{
    variant<mock_value> a((mock_value()));
    assert_log_message(log_level::info, "mock_value default constructor");
    assert_log_message(log_level::info, "mock_value move constructor");
    assert_log_message(log_level::info, "mock_value destructor");
    assert_log_empty();

    a = mock_value();
    assert_log_message(log_level::info, "mock_value default constructor");
    assert_log_message(log_level::info, "mock_value destructor");
    assert_log_message(log_level::info, "mock_value move constructor");
    assert_log_message(log_level::info, "mock_value destructor");
    assert_log_empty();
}

test_case(value_copy)
{
    mock_value v;
    assert_log_message(log_level::info, "mock_value default constructor");
    assert_log_empty();

    variant<mock_value> a(v);
    assert_log_message(log_level::info, "mock_value copy constructor");
    assert_log_empty();

    a = v;
    assert_log_message(log_level::info, "mock_value destructor");
    assert_log_message(log_level::info, "mock_value copy constructor");
    assert_log_empty();
}

test_case(copy_same)
{
    class mock_value_visitor {
    public:
        mock_value const& visit(mock_value const &v) const
        {
            return v;
        }
    };

    mock_value v;
    assert_log_message(log_level::info, "mock_value default constructor");
    assert_log_empty();

    mock_value const &t = v;
    variant<mock_value> a(t);
    assert_log_message(log_level::info, "mock_value copy constructor");
    assert_log_empty();

    auto const &w = a.accept(mock_value_visitor());
    a = w;
    assert_log_empty();

    mock_value x;
    assert_log_message(log_level::info, "mock_value default constructor");
    assert_log_empty();

    auto const &y = x;
    a = y;
    assert_log_message(log_level::info, "mock_value destructor");
    assert_log_message(log_level::info, "mock_value copy constructor");
    assert_log_empty();
}

test_case(variant_copy_ctor)
{
    variant<int> v(65);
    auto const &v_ref = v;

    variant<int> w(v_ref);
    assert_eq(w.accept(int_collector_visitor()), 65);

    variant<int> t(5);
    auto const &t_ref = t;

    w = t_ref;
    assert_eq(w.accept(int_collector_visitor()), 5);
}

test_case(variant_move_ctor)
{
    variant<std::unique_ptr<int>> v(std::make_unique<int>(58));
    variant<std::unique_ptr<int>> w(std::make_unique<int>(274));

    variant<std::unique_ptr<int>> x(std::move(v));
    assert_eq(x.accept(int_collector_visitor()), 58);

    x = std::move(w);
    assert_eq(x.accept(int_collector_visitor()), 274);
}

test_case(variant_vector)
{
    using myvar = variant<int, std::string>;

    std::vector<myvar> values;
    values.emplace_back(myvar(5));
    values.emplace_back(std::string("Hello, World!"));

    myvar v = myvar(6);
    values.push_back(v);
}

end_suite(variant_test);
