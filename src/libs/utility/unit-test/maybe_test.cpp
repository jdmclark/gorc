#include "test/test.hpp"
#include "utility/maybe.hpp"
#include "log/log.hpp"

using namespace gorc;

begin_suite(maybe_test);

test_case(pointer_values)
{
    char const *hello = "Hello, World!";

    maybe<char const *> m;
    assert_true(!m.has_value());

    m = hello;
    assert_true(m.has_value());
    assert_eq(m.get_value(), hello);

    m = nothing;
    assert_true(!m.has_value());
}

test_case(pointer_value_size)
{
    char const *hello = "Hello, World!";
    maybe<char const *> m;

    assert_eq(sizeof(hello), sizeof(m));
}

test_case(nullptr_equality)
{
    maybe<char const *> m;
    maybe<char const *> n;

    assert_eq(m, n);
    assert_eq(m, nullptr);
    assert_eq(m, nothing);

    assert_true(!(m != n));
    assert_true(!(m != nullptr));
    assert_true(!(m != nothing));
}

test_case(nullptr_inequality)
{
    char const *hello = "Hello, World!";
    maybe<char const *> m = hello;
    maybe<char const *> n;

    assert_ne(m, n);
    assert_ne(m, nullptr);
    assert_ne(m, nothing);

    assert_true(!(m == n));
    assert_true(!(m == nullptr));
    assert_true(!(m == nothing));
}

test_case(pointer_value_equality)
{
    char const *hello = "Hello, World!";
    maybe<char const *> m = hello;
    maybe<char const *> n = hello;

    assert_eq(m, n);
    assert_eq(m, hello);

    assert_true(!(m != n));
    assert_true(!(m != hello));
}

test_case(pointer_value_inequality)
{
    char const *hello = "Hello, World!";
    char const *other = "Other";
    maybe<char const *> m = hello;
    maybe<char const *> n = other;

    assert_ne(m, n);
    assert_ne(m, other);

    assert_true(!(m == n));
    assert_true(!(m == other));
}

test_case(pointer_get_value_const)
{
    maybe<char const *> const m = "Hello, World!";
    assert_eq(m.get_value(), std::string("Hello, World!"));
}

test_case(pointer_get_value_write_back)
{
    char const *hello = "Hello, World!";
    char const *other = "Other";

    maybe<char const *> m = hello;

    m.get_value() = other;

    assert_eq(m.get_value(), other);
}

test_case(object_values)
{
    int value = 5;

    maybe<int> m;
    assert_true(!m.has_value());

    m = value;
    assert_true(m.has_value());
    assert_eq(m.get_value(), value);

    m = nothing;
    assert_true(!m.has_value());
}

test_case(object_equality)
{
    maybe<int> m(5);
    maybe<int> n(5);

    assert_eq(m, n);
    assert_eq(m, 5);

    assert_true(!(m != n));
    assert_true(!(m != 5));
}

test_case(non_value_object_equality)
{
    maybe<int> m;
    maybe<int> n;

    assert_eq(m, n);
    assert_eq(m, nothing);

    assert_true(!(m != n));
    assert_true(!(m != nothing));
}

test_case(object_inequality)
{
    maybe<int> m(5);
    maybe<int> n(6);

    assert_ne(m, n);
    assert_ne(m, 6);

    assert_true(!(m == n));
    assert_true(!(m == 6));
}

test_case(non_value_object_inequality)
{
    maybe<int> m(5);
    maybe<int> n;

    assert_ne(m, n);
    assert_ne(n, m);

    assert_true(!(m == n));
    assert_true(!(n == m));
}

test_case(object_get_value_const)
{
    maybe<int> const m(5);

    assert_eq(m.get_value(), 5);
}

test_case(object_get_value_write_back)
{
    maybe<int> m(5);

    m.get_value() = 156;

    assert_eq(m.get_value(), 156);
}

test_case(make_maybe)
{
    auto m = make_maybe(5);

    assert_true(m.has_value());
    assert_eq(m.get_value(), 5);
}

test_case(maybe_if_void)
{
    bool called = false;
    int value = 0;

    auto lambda = [&](int x) { called = true; value = x; };

    maybe<int> m;
    maybe_if(m, lambda);

    assert_true(!called);

    maybe<int> const n = 63;
    maybe_if(n, lambda);

    assert_true(called);
    assert_eq(value, 63);
}

test_case(maybe_if_default_constructor)
{
    class called_context {
    public:
        bool called = false;
        int value = 0;

        called_context() = default;

        called_context(bool called, int value)
            : called(called)
            , value(value)
        {
            return;
        }
    };

    auto lambda = [&](int x) { return called_context(true, x); };

    maybe<int> m;
    auto c1 = maybe_if(m, lambda);
    assert_true(!c1.called);

    maybe<int> n = 63;
    auto c2 = maybe_if(n, lambda);

    assert_true(c2.called);
    assert_eq(c2.value, 63);
}

test_case(maybe_if_void_write_back)
{
    maybe<int> m = 58;
    maybe_if(m, [](int &x) { x = 8573; });

    assert_true(m.has_value());
    assert_eq(m.get_value(), 8573);
}

test_case(maybe_if_default_value)
{
    auto lambda = [](int x) { return x; };

    maybe<int> m;
    assert_eq(maybe_if(m, 48773, lambda), 48773);

    maybe<int> n = 5;
    assert_eq(maybe_if(n, 48773, lambda), 5);
}

test_case(maybe_if_else)
{
    class called_context {
    public:
        bool if_taken = false;
        bool else_taken = false;
        int value = 0;

        called_context(bool if_taken, bool else_taken, int value)
            : if_taken(if_taken)
            , else_taken(else_taken)
            , value(value)
        {
            return;
        }
    };

    auto if_fn = [](int x) { return called_context(true, false, x); };
    auto else_fn = [] { return called_context(false, true, 0); };

    maybe<int> m;
    auto c1 = maybe_if_else(m, if_fn, else_fn);

    assert_true(!c1.if_taken);
    assert_true(c1.else_taken);

    maybe<int> n = 5;
    auto c2 = maybe_if_else(n, if_fn, else_fn);

    assert_true(c2.if_taken);
    assert_true(!c2.else_taken);
    assert_eq(c2.value, 5);
}

test_case(object_ctor_dtor_calls)
{
    class complicated_object {
    public:
        int value;

        complicated_object(int value)
            : value(value)
        {
            LOG_INFO("constructor called");
            return;
        }

        complicated_object(complicated_object const &obj)
            : value(obj.value)
        {
            LOG_INFO("copy constructor called");
            return;
        }

        ~complicated_object()
        {
            LOG_INFO("destructor called");
        }

        bool operator==(complicated_object const &obj) const
        {
            return value == obj.value;
        }
    };

    complicated_object obj_a(5);
    complicated_object obj_b(6);

    assert_log_message(log_level::info, "constructor called");
    assert_log_message(log_level::info, "constructor called");
    assert_log_empty();

    maybe<complicated_object> m;
    assert_log_empty();

    m = obj_a;
    assert_log_message(log_level::info, "copy constructor called");
    assert_log_empty();

    assert_true(m == obj_a);
    assert_log_empty();

    m = obj_b;
    assert_log_message(log_level::info, "destructor called");
    assert_log_message(log_level::info, "copy constructor called");
    assert_log_empty();

    assert_true(m == obj_b);
    assert_log_empty();

    m = m;
    assert_log_empty();

    m = m.get_value();
    assert_log_empty();

    m = nothing;
    assert_log_message(log_level::info, "destructor called");
    assert_log_empty();

    maybe<complicated_object> n(obj_a);
    assert_log_message(log_level::info, "copy constructor called");
    assert_log_empty();

    m = n;
    assert_log_message(log_level::info, "copy constructor called");
    assert_log_empty();
    assert_true(m.has_value());
    assert_true(m == n);

    n = nothing;
    assert_log_message(log_level::info, "destructor called");
    assert_log_empty();

    m = n;
    assert_log_message(log_level::info, "destructor called");
    assert_log_empty();
    assert_true(!m.has_value());

    m = maybe<complicated_object>();
    assert_log_empty();
    assert_true(!m.has_value());

    m = maybe<complicated_object>(5);
    assert_log_message(log_level::info, "constructor called");
    assert_log_message(log_level::info, "copy constructor called");
    assert_log_message(log_level::info, "copy constructor called");
    assert_log_message(log_level::info, "destructor called");
    assert_log_message(log_level::info, "destructor called");
    assert_log_empty();
    assert_true(m.has_value());
}

test_case(uncopyable)
{
    maybe<std::unique_ptr<int>> m;

    m = std::make_unique<int>(152);

    assert_eq(*m.get_value(), 152);
}

test_case(uncopyable_ctor)
{
    maybe<std::unique_ptr<int>> m(std::make_unique<int>(8573));
    assert_eq(*m.get_value(), 8573);
}

test_case(maybe_str)
{
    auto get_maybe = []()-> maybe<std::string> {
        char const *value = "some_string";
        return make_maybe<std::string>(value);
    };

    maybe<std::string> maybe_some_string = get_maybe();
    assert_true(maybe_some_string.has_value());
    assert_eq(maybe_some_string.get_value(), std::string("some_string"));
}

test_case(maybe_value)
{
    maybe<int> m = 5;
    maybe<int> n = nothing;

    assert_eq(maybe_value(m, 12), 5);
    assert_eq(maybe_value(n, 12), 12);
}

end_suite(maybe_test);
