#include "test/test.hpp"
#include "content/content_manager.hpp"
#include "vfs/virtual_file_system.hpp"
#include "io/memory_file.hpp"
#include "content/loader.hpp"
#include "content/loader_registry.hpp"
#include "log/log.hpp"

using namespace gorc;

namespace {

    class mock_vfs : public virtual_file_system {
    private:
        memory_file foo_mf;
        memory_file bar_mf;
        memory_file default_mf;

    public:
        mock_vfs()
        {
            int value = 5;
            foo_mf.write(&value, sizeof(int));

            value = 10;
            bar_mf.write(&value, sizeof(int));

            value = 58;
            default_mf.write(&value, sizeof(int));
        }

        virtual std::unique_ptr<input_stream> open(path const &p) const override
        {
            if(p == "foo") {
                return std::make_unique<memory_file::reader>(foo_mf);
            }
            else if(p == "bar") {
                return std::make_unique<memory_file::reader>(bar_mf);
            }
            else {
                return std::make_unique<memory_file::reader>(default_mf);
            }
        }
    };

    class mock_asset : public asset {
    public:
        static fourcc const type;

        int value;

        mock_asset(int value)
            : value(value)
        {
            return;
        }
    };

    fourcc const mock_asset::type = "MOCK"_4CC;

    class mock_loader : public loader {
    public:
        virtual std::unique_ptr<asset> deserialize(input_stream &is,
                                                   content_manager &,
                                                   service_registry const &) const override
        {
            LOG_INFO("called mock_loader");
            int value;
            is.read(&value, sizeof(int));
            return std::make_unique<mock_asset>(value);
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

test_case(simple_load)
{
    content_manager content(services);

    auto foo_ref = content.load<mock_asset>("foo");
    assert_log_message(log_level::info, "called mock_loader");
    assert_log_empty();

    assert_eq(foo_ref->value, 5);

    auto bar_ref = content.load<mock_asset>("bar");
    assert_log_message(log_level::info, "called mock_loader");
    assert_log_empty();

    assert_eq(bar_ref->value, 10);
}

test_case(name_instance_reuse)
{
    content_manager content(services);

    auto foo_ref = content.load<mock_asset>("foo");
    assert_log_message(log_level::info, "called mock_loader");
    assert_log_empty();

    auto bar_ref = content.load<mock_asset>("bar");
    assert_log_message(log_level::info, "called mock_loader");
    assert_log_empty();

    auto foo2_ref = content.load<mock_asset>("foo");
    assert_log_empty();

    auto bar2_ref = content.load<mock_asset>("bar");
    assert_log_empty();

    assert_eq(&(*foo_ref), &(*foo2_ref));
    assert_eq(&(*bar_ref), &(*bar2_ref));
}

end_suite(content_manager_test);
