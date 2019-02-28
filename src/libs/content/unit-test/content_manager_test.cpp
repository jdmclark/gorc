#include "content/content_manager.hpp"
#include "content/loader.hpp"
#include "content/loader_registry.hpp"
#include "io/memory_file.hpp"
#include "log/log.hpp"
#include "test/test.hpp"
#include "vfs/virtual_file_system.hpp"

using namespace gorc;

namespace {

    class mock_vfs : public virtual_file_system {
    private:
        memory_file foo_mf;
        memory_file bar_mf;
        memory_file fnord_mf;
        memory_file default_mf;

    public:
        mock_vfs()
        {
            int value = 5;
            foo_mf.write(&value, sizeof(int));

            value = 10;
            bar_mf.write(&value, sizeof(int));

            value = 58;
            fnord_mf.write(&value, sizeof(int));

            value = 23;
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
            else if(p == "fnord") {
                return std::make_unique<memory_file::reader>(fnord_mf);
            }
            else if(p == "dflt") {
                return std::make_unique<memory_file::reader>(default_mf);
            }

            LOG_FATAL(format("could not open") % p.generic_string());
        }

        virtual std::tuple<path, std::unique_ptr<input_stream>>
            find(path const &p, std::vector<path> const &) const override
        {
            return std::make_tuple(p, open(p));
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
        static fourcc const type;

        virtual std::vector<path> const &get_prefixes() const override
        {
            static std::vector<path> rv = {""};
            return rv;
        }

        virtual maybe<char const *> get_default() const override
        {
            return "dflt";
        }

        virtual std::unique_ptr<asset> deserialize(input_stream &is,
                                                   content_manager &,
                                                   asset_id id,
                                                   service_registry const &,
                                                   std::string const &) const override
        {
            LOG_INFO(format("called mock_loader for asset %d") % static_cast<int>(id));
            int value;
            is.read(&value, sizeof(int));
            return std::make_unique<mock_asset>(value);
        }
    };

    MAKE_ID_TYPE(mock_asset);

    fourcc const mock_loader::type = "MOCK"_4CC;
}

namespace gorc {

    template <>
    struct id_asset_type<mock_asset_id> {
        using type = mock_asset;
    };
}

class content_manager_test_fixture : public test::fixture {
public:
    service_registry services;
    loader_registry loaders;
    mock_vfs vfs;

    content_manager_test_fixture()
    {
        loaders.emplace_loader<mock_loader>();
        services.add(loaders);
        services.add<virtual_file_system>(vfs);
    }
};

begin_suite_fixture(content_manager_test, content_manager_test_fixture);

test_case(simple_load)
{
    content_manager content(services);

    auto foo_ref = content.load<mock_asset>("foo");
    assert_log_message(log_level::info, "foo: called mock_loader for asset 0");
    assert_log_empty();

    assert_eq(foo_ref->value, 5);

    auto bar_ref = content.load<mock_asset>("bar");
    assert_log_message(log_level::info, "bar: called mock_loader for asset 1");
    assert_log_empty();

    assert_eq(bar_ref->value, 10);
}

test_case(default_load)
{
    content_manager content(services);

    auto dne_ref = content.load<mock_asset>("dne");
    assert_log_message(log_level::error, "dne: failed to load asset dne, using default instead");
    assert_log_message(log_level::info, "dflt: called mock_loader for asset 0");
    assert_log_empty();

    assert_eq(dne_ref->value, 23);
}

test_case(name_instance_reuse)
{
    content_manager content(services);

    auto foo_ref = content.load<mock_asset>("foo");
    assert_log_message(log_level::info, "foo: called mock_loader for asset 0");
    assert_log_empty();

    auto bar_ref = content.load<mock_asset>("bar");
    assert_log_message(log_level::info, "bar: called mock_loader for asset 1");
    assert_log_empty();

    auto foo2_ref = content.load<mock_asset>("foo");
    assert_log_empty();

    auto bar2_ref = content.load<mock_asset>("bar");
    assert_log_empty();

    assert_eq(&(*foo_ref), &(*foo2_ref));
    assert_eq(&(*bar_ref), &(*bar2_ref));
}

test_case(freeze_thaw_content_references)
{
    memory_file mf;

    {
        // Prime content manager
        content_manager content(services);
        auto bar_ref = content.load<mock_asset>("bar");
        assert_log_message(log_level::info, "bar: called mock_loader for asset 0");
        assert_log_empty();
        auto fnord_ref = content.load<mock_asset>("fnord");
        assert_log_message(log_level::info, "fnord: called mock_loader for asset 1");
        assert_log_empty();
        auto foo_ref = content.load<mock_asset>("foo");
        assert_log_message(log_level::info, "foo: called mock_loader for asset 2");
        assert_log_empty();

        binary_output_stream bos(mf);
        binary_serialize(bos, content);
        binary_serialize(bos, foo_ref);
        binary_serialize(bos, bar_ref);
        binary_serialize(bos, fnord_ref);
    }

    {
        binary_input_stream bis(mf, services);
        auto content = binary_deserialize<content_manager>(bis);
        assert_log_empty();

        // Content manager must be added to the service registry from this point
        services.add(content);

        auto foo_ref = binary_deserialize<asset_ref<mock_asset>>(bis);
        assert_eq(foo_ref->value, 5);
        assert_log_message(log_level::info, "foo: called mock_loader for asset 2");
        assert_log_empty();

        auto bar_ref = binary_deserialize<asset_ref<mock_asset>>(bis);
        assert_eq(bar_ref->value, 10);
        assert_log_message(log_level::info, "bar: called mock_loader for asset 0");
        assert_log_empty();

        auto fnord_ref = binary_deserialize<asset_ref<mock_asset>>(bis);
        assert_eq(fnord_ref->value, 58);
        assert_log_message(log_level::info, "fnord: called mock_loader for asset 1");
        assert_log_empty();
    }
}

test_case(get_asset)
{
    content_manager content(services);
    auto foo_ref = content.load<mock_asset>("foo");

    mock_asset_id mai(static_cast<int>(foo_ref.get_id()));

    auto foo_ref2 = get_asset(content, mai);

    assert_eq(foo_ref, foo_ref2);
}

end_suite(content_manager_test);
