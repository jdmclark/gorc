#include "test/test.hpp"
#include "content/loader_registry.hpp"

using namespace gorc;

namespace {

    class mock_loader : public loader {
    public:
        int value;

        mock_loader(int value)
            : value(value)
        {
            return;
        }

        virtual std::unique_ptr<asset> deserialize(input_stream&,
                                                   content_manager&,
                                                   service_registry const&) const override
        {
            return nullptr;
        }
    };

}

begin_suite(loader_registry_test);

test_case(simple_emplace_get)
{
    loader_registry lr;
    lr.emplace_loader<mock_loader>("MOCK"_4CC, 58);

    auto const &ldr = lr.get_loader("MOCK"_4CC);
    assert_eq(dynamic_cast<mock_loader const&>(ldr).value, 58);
}

test_case(no_loader_exists)
{
    loader_registry lr;

    assert_throws_logged(lr.get_loader("DNE"_4CC));
    assert_log_message(log_level::error, "no loader for content type DNE");
    assert_log_empty();
}

end_suite(loader_registry_test);
