#include "test/test.hpp"
#include "content/content_manager.hpp"
#include "vfs/virtual_file_system.hpp"
#include "io/std_input_stream.hpp"
#include "content/loader.hpp"
#include "content/loader_registry.hpp"

using namespace gorc;

namespace {

    class mock_vfs : public virtual_file_system {
    public:
        virtual std::unique_ptr<input_stream> open(path const &) const override
        {
            return std::make_unique<std_input_stream>();
        }
    };

    class mock_asset : public asset {
    public:
        static fourcc const type;
    };

    fourcc const mock_asset::type = "MOCK"_4CC;

    class mock_loader : public loader {
    public:
        virtual std::unique_ptr<asset> deserialize(input_stream &,
                                                   content_manager &,
                                                   service_registry const &) const override
        {
            return std::make_unique<mock_asset>();
        }
    };

}

class content_manager_test_fixture : public test::fixture {
public:
    service_registry services;
    loader_registry loaders;
    mock_vfs vfs;

    content_manager_test_fixture()
    {
        loaders.emplace_loader<mock_loader>("MOCK");
        services.add(loaders);
        services.add<virtual_file_system>(vfs);
    }
};

begin_suite_fixture(content_manager_test, content_manager_test_fixture);

test_case(name_instance_reuse)
{
    content_manager content(services);

    auto foo_ref = content.load<mock_asset>("foo");
    auto bar_ref = content.load<mock_asset>("bar");

    auto foo2_ref = content.load<mock_asset>("foo");
    auto bar2_ref = content.load<mock_asset>("bar");

    assert_eq(&(*foo_ref), &(*foo2_ref));
    assert_eq(&(*bar_ref), &(*bar2_ref));
}

end_suite(content_manager_test);
