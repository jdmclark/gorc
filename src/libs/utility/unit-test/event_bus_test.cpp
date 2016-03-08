#include "test/test.hpp"
#include "utility/event_bus.hpp"
#include "log/log.hpp"

using namespace gorc;

class mock_event {
public:
    int value;

    mock_event(int value)
        : value(value)
    {
        return;
    }
};

begin_suite(event_bus_test);

test_case(simple)
{
    event_bus bus;

    auto handler_guard = bus.add_handler<mock_event>([](auto const &e) {
            LOG_INFO(format("simple handler: %d") % e.value);
        });

    assert_log_empty();

    bus.fire_event(mock_event(15));

    assert_log_message(log_level::info, "simple handler: 15");
    assert_log_empty();
}

test_case(guard_unregisters)
{
    event_bus bus;

    {
        auto handler_guard = bus.add_handler<mock_event>([](auto const &e) {
                LOG_INFO(format("simple handler: %d") % e.value);
            });

        assert_log_empty();

        bus.fire_event(mock_event(20));

        assert_log_message(log_level::info, "simple handler: 20");
        assert_log_empty();
    }

    bus.fire_event(mock_event(13));
    assert_log_empty();
}

test_case(const_called_after_mutable)
{
    event_bus bus;

    auto handler_guard = bus.add_handler<mock_event>([](auto const &e) {
            LOG_INFO(format("const handler: %d") % e.value);
        });

    auto var_handler_guard = bus.add_handler<mock_event>([](mock_event &e) {
            LOG_INFO(format("var handler: %d") % e.value);
            e.value *= 2;
        });

    assert_log_empty();

    mock_event evt(12);
    bus.fire_event(evt);
    assert_log_message(log_level::info, "var handler: 12");
    assert_log_message(log_level::info, "const handler: 24");
    assert_log_empty();
}

test_case(const_dispatch_with_mutable_handlers_fatal)
{
    event_bus bus;

    auto handler_guard = bus.add_handler<mock_event>([](auto const &e) {
            LOG_INFO(format("const handler: %d") % e.value);
        });

    auto var_handler_guard = bus.add_handler<mock_event>([](mock_event &e) {
            LOG_INFO(format("var handler: %d") % e.value);
            e.value *= 2;
        });

    assert_throws(bus.fire_event(mock_event(12)),
                  std::runtime_error,
                  strcat("fatal: dispatched constant event when "
                         "non-constant handlers are registered: ",
                         typeid(mock_event).name()));
}


test_case(register_reuses_ids)
{
    event_bus bus;

    auto handler_1 = bus.add_handler<mock_event>([](auto const &e) {
            LOG_INFO(format("handler 1: %d") % e.value);
        });

    auto handler_2 = bus.add_handler<mock_event>([](auto const &e) {
            LOG_INFO(format("handler 2: %d") % e.value);
        });

    maybe<scoped_delegate> handler_3 = bus.add_handler<mock_event>([](auto const &e) {
            LOG_INFO(format("handler 3: %d") % e.value);
        });

    auto handler_4 = bus.add_handler<mock_event>([](auto const &e) {
            LOG_INFO(format("handler 4: %d") % e.value);
        });

    assert_log_empty();

    bus.fire_event(mock_event(5));

    assert_log_message(log_level::info, "handler 1: 5");
    assert_log_message(log_level::info, "handler 2: 5");
    assert_log_message(log_level::info, "handler 3: 5");
    assert_log_message(log_level::info, "handler 4: 5");
    assert_log_empty();

    handler_3 = nothing;

    bus.fire_event(mock_event(6));

    assert_log_message(log_level::info, "handler 1: 6");
    assert_log_message(log_level::info, "handler 2: 6");
    assert_log_message(log_level::info, "handler 4: 6");
    assert_log_empty();

    auto handler_5 = bus.add_handler<mock_event>([](auto const &e) {
            LOG_INFO(format("handler 5: %d") % e.value);
        });

    bus.fire_event(mock_event(7));

    assert_log_message(log_level::info, "handler 1: 7");
    assert_log_message(log_level::info, "handler 2: 7");
    assert_log_message(log_level::info, "handler 5: 7");
    assert_log_message(log_level::info, "handler 4: 7");
    assert_log_empty();
}

test_case(reentrant_register_no_id_reuse)
{
    event_bus bus;

    auto handler_1 = bus.add_handler<mock_event>([](auto const &e) {
            LOG_INFO(format("handler 1: %d") % e.value);
        });

    auto handler_2 = bus.add_handler<mock_event>([](auto const &e) {
            LOG_INFO(format("handler 2: %d") % e.value);
        });

    auto handler_3 = bus.add_handler<mock_event>([](auto const &e) {
            LOG_INFO(format("handler 3: %d") % e.value);
        });

    maybe<scoped_delegate> handler_4 = bus.add_handler<mock_event>([&](auto const &e) {
            LOG_INFO(format("handler 4: %d") % e.value);
            handler_4 = nothing;
            auto handler_6 = bus.add_handler<mock_event>([](auto const &e) {
                    LOG_INFO(format("handler 6: %d") % e.value);
                });
            bus.fire_event(mock_event(8));
        });

    auto handler_5 = bus.add_handler<mock_event>([](auto const &e) {
            LOG_INFO(format("handler 5: %d") % e.value);
        });

    assert_log_empty();

    bus.fire_event(mock_event(5));

    assert_log_message(log_level::info, "handler 1: 5");
    assert_log_message(log_level::info, "handler 2: 5");
    assert_log_message(log_level::info, "handler 3: 5");
    assert_log_message(log_level::info, "handler 4: 5");
    assert_log_message(log_level::info, "handler 1: 8");
    assert_log_message(log_level::info, "handler 2: 8");
    assert_log_message(log_level::info, "handler 3: 8");
    assert_log_message(log_level::info, "handler 5: 8");
    assert_log_message(log_level::info, "handler 6: 8");
    assert_log_message(log_level::info, "handler 5: 5");
    assert_log_empty();
}

test_case(scoped_delegate_overwrite)
{
    event_bus bus;

    {
        auto handler = bus.add_handler<mock_event>([](auto const &e) {
                LOG_INFO(format("handler 1: %d") % e.value);
            });

        bus.fire_event(mock_event(5));

        assert_log_message(log_level::info, "handler 1: 5");
        assert_log_empty();

        {
            handler = bus.add_handler<mock_event>([](auto const &e) {
                    LOG_INFO(format("handler 2: %d") % e.value);
                });

            bus.fire_event(mock_event(7));

            assert_log_message(log_level::info, "handler 2: 7");
            assert_log_empty();

        }

        bus.fire_event(mock_event(13));

        assert_log_message(log_level::info, "handler 2: 13");
        assert_log_empty();
    }

    bus.fire_event(mock_event(11));

    assert_log_empty();
}

end_suite(event_bus_test);
