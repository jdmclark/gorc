#include "test/test.hpp"
#include "log/log.hpp"
#include <thread>

using namespace gorc;

begin_suite(diagnostic_context_test);

test_case(filename_only)
{
    char const *p("/usr/local/bin/foo.bar");
    gorc::diagnostic_context diagnostic_context(p);

    LOG_ERROR("some message");

    assert_log_message(gorc::log_level::error, "/usr/local/bin/foo.bar: some message");
    assert_log_empty();
}

test_case(filename_line_only)
{
    char const *p("/usr/local/bin/foo.bar");
    gorc::diagnostic_context diagnostic_context(p, 5);

    LOG_ERROR("some message");

    assert_log_message(gorc::log_level::error, "/usr/local/bin/foo.bar:5: some message");
    assert_log_empty();
}

test_case(filename_line_col)
{
    char const *p("/usr/local/bin/foo.bar");
    gorc::diagnostic_context diagnostic_context(p, 5, 10);

    LOG_ERROR("some message");

    assert_log_message(gorc::log_level::error, "/usr/local/bin/foo.bar:5:10: some message");
    assert_log_empty();
}

test_case(filename_col_only)
{
    char const *p("/usr/local/bin/foo.bar");
    gorc::diagnostic_context diagnostic_context(p, 0, 5);

    LOG_ERROR("some message");

    assert_log_message(gorc::log_level::error, "/usr/local/bin/foo.bar: some message");
    assert_log_empty();
}

test_case(no_filename)
{
    gorc::diagnostic_context diagnostic_context(gorc::nothing);

    LOG_ERROR("some message");

    assert_log_message(gorc::log_level::error, "<BUFFER>: some message");
    assert_log_empty();
}

test_case(no_filename_line_col)
{
    gorc::diagnostic_context diagnostic_context(gorc::nothing, 5, 10);

    LOG_ERROR("some message");

    assert_log_message(gorc::log_level::error, "<BUFFER>:5:10: some message");
    assert_log_empty();
}

test_case(end_line_only)
{
    gorc::diagnostic_context diagnostic_context(gorc::nothing, 5, 10, 12);

    LOG_ERROR("some message");

    assert_log_message(gorc::log_level::error, "<BUFFER>:5:10-12: some message");
}

test_case(end_line_col)
{
    gorc::diagnostic_context diagnostic_context(gorc::nothing, 5, 10, 12, 8);

    LOG_ERROR("some message");

    assert_log_message(gorc::log_level::error, "<BUFFER>:5:10-12:8: some message");
}

test_case(nested_contexts)
{
    auto inner_file = []() {
        char const *p("/usr/local/bin/baz.bar");
        gorc::diagnostic_context diagnostic_context(p, 1, 64);

        LOG_ERROR("inner_file");
    };

    auto outer_file = [&]() {
        char const *p("/usr/local/bin/foo.bar");
        gorc::diagnostic_context diagnostic_context(p);

        LOG_ERROR("outer_file outside scope");

        if(true) {
            gorc::diagnostic_context inner_context(gorc::nothing, 5);
            LOG_ERROR("outer_file inside scope");

            inner_file();
        }

        LOG_ERROR("outer_file after");
    };

    LOG_ERROR("outside file");
    outer_file();
    LOG_ERROR("back outside");

    assert_log_message(gorc::log_level::error, "outside file");
    assert_log_message(gorc::log_level::error, "/usr/local/bin/foo.bar: outer_file outside scope");
    assert_log_message(gorc::log_level::error, "/usr/local/bin/foo.bar:5: outer_file inside scope");
    assert_log_message(gorc::log_level::error, "/usr/local/bin/baz.bar:1:64: inner_file");
    assert_log_message(gorc::log_level::error, "/usr/local/bin/foo.bar: outer_file after");
    assert_log_message(gorc::log_level::error, "back outside");
    assert_log_empty();
}

test_case(parallel_contexts)
{
    char const *p("/usr/local/bin/baz.bar");
    gorc::diagnostic_context dc(p);

    auto thread_proc = []() {
        LOG_ERROR("Inner");

        char const *p("/usr/local/bin/foo.bar");
        gorc::diagnostic_context dc(p);

        LOG_ERROR("Inner context");
    };

    LOG_ERROR("Outer");

    std::thread th(thread_proc);
    th.join();

    LOG_ERROR("After");

    assert_log_message(gorc::log_level::error, "/usr/local/bin/baz.bar: Outer");
    assert_log_message(gorc::log_level::error, "Inner");
    assert_log_message(gorc::log_level::error, "/usr/local/bin/foo.bar: Inner context");
    assert_log_message(gorc::log_level::error, "/usr/local/bin/baz.bar: After");
    assert_log_empty();
}

test_case(error_count_no_file)
{
    assert_eq(gorc::diagnostic_file_error_count(), 0);

    LOG_ERROR("foo");

    assert_eq(gorc::diagnostic_file_error_count(), 0);
}

test_case(error_count_includes_errors_only)
{
    gorc::diagnostic_context dc(gorc::nothing);

    LOG_TRACE("foo");
    LOG_DEBUG("foo");
    LOG_INFO("foo");
    LOG_WARNING("foo");

    assert_eq(gorc::diagnostic_file_error_count(), 0);

    LOG_ERROR("foo");

    assert_eq(gorc::diagnostic_file_error_count(), 1);
}

test_case(error_count_excludes_child)
{
    gorc::diagnostic_context dc("foo");

    LOG_ERROR("foo");

    assert_eq(gorc::diagnostic_file_error_count(), 1);

    do {
        gorc::diagnostic_context dd("bar");
        assert_eq(gorc::diagnostic_file_error_count(), 0);
        LOG_ERROR("bar");
        LOG_ERROR("baz");
        assert_eq(gorc::diagnostic_file_error_count(), 2);
    } while(false);

    assert_eq(gorc::diagnostic_file_error_count(), 1);
}

test_case(error_count_accumulates_nested)
{
    gorc::diagnostic_context dc("foo");

    LOG_ERROR("foo");
    LOG_ERROR("bar");

    do {
        gorc::diagnostic_context dd(gorc::nothing, 5, 10);
        assert_eq(gorc::diagnostic_file_error_count(), 2);

        LOG_ERROR("baz");
        LOG_ERROR("fnord");

        assert_eq(gorc::diagnostic_file_error_count(), 4);
    } while(false);

    assert_eq(gorc::diagnostic_file_error_count(), 4);
}

test_case(diagnostic_context_location_casts)
{
    gorc::diagnostic_context_location dcl("foo", 5, 7);
    gorc::diagnostic_context dc(dcl);

    LOG_ERROR("baz");

    assert_log_message(gorc::log_level::error, "foo:5:7: baz");
    assert_log_empty();
}

test_case(diagnostic_context_location_default_constructor)
{
    gorc::diagnostic_context_location dcl;
    assert_eq(dcl.filename, gorc::nothing);
    assert_eq(dcl.first_line, 0);
    assert_eq(dcl.first_col, 0);
    assert_eq(dcl.last_line, 0);
    assert_eq(dcl.last_col, 0);
}

test_case(unordered_destruction)
{
    auto dcl = std::make_unique<gorc::diagnostic_context>("foo");
    auto dcm = std::make_unique<gorc::diagnostic_context>("bar");

    LOG_ERROR("first message");
    dcl.reset();

    LOG_ERROR("second message");

    assert_log_message(gorc::log_level::error, "bar: first message");
    assert_log_message(gorc::log_level::error, "bar: second message");
    assert_log_empty();
}

test_case(diagnostic_location_equality)
{
    char const *fn = "foo";
    char const *gn = "bar";

    diagnostic_context_location a(fn, 5, 10, 15, 20);

    assert_true(a == diagnostic_context_location(fn, 5, 10, 15, 20));

    assert_true(!(a == diagnostic_context_location(gn, 5, 10, 15, 20)));
    assert_true(!(a == diagnostic_context_location(fn, 6, 10, 15, 20)));
    assert_true(!(a == diagnostic_context_location(fn, 5, 11, 15, 20)));
    assert_true(!(a == diagnostic_context_location(fn, 5, 10, 16, 20)));
    assert_true(!(a == diagnostic_context_location(fn, 5, 10, 15, 21)));
}

end_suite(diagnostic_context_test);
