#include "test/test.hpp"
#include "vfs/virtual_container.hpp"
#include "log/log.hpp"

using namespace gorc;

class mock_virtual_file : public virtual_file {
public:
    mock_virtual_file(path const &fn)
        : virtual_file(fn)
    {
        return;
    }

    virtual std::unique_ptr<read_only_file> open() const override
    {
        LOG_FATAL("unimplemented");
    }
};

class mock_virtual_container : public virtual_container {
private:
    std::vector<mock_virtual_file> files;

protected:
    virtual virtual_file const& get_file(size_t index) const override
    {
        return files[index];
    }

public:
    virtual size_t size() const override
    {
        return files.size();
    }

    void add_file(std::string const &fn)
    {
        files.emplace_back(fn);
    }
};

begin_suite(virtual_container_iterator_test);

test_case(empty_container)
{
    mock_virtual_container cnt;
    assert_eq(cnt.size(), size_t(0));

    assert_eq(cnt.begin(), cnt.end());
}

test_case(simple_iterate)
{
    mock_virtual_container cnt;
    cnt.add_file("foo");
    cnt.add_file("bar");
    cnt.add_file("baz");

    for(auto const &file : cnt) {
        LOG_INFO(file.name.generic_string());
    }

    assert_log_message(log_level::info, "foo");
    assert_log_message(log_level::info, "bar");
    assert_log_message(log_level::info, "baz");
    assert_log_empty();
}

test_case(simple_iterate_2)
{
    mock_virtual_container cnt;
    cnt.add_file("foo");
    cnt.add_file("bar");
    cnt.add_file("baz");

    for(auto it = cnt.begin(); it != cnt.end(); ++it) {
        LOG_INFO(it->name.generic_string());
    }

    assert_log_message(log_level::info, "foo");
    assert_log_message(log_level::info, "bar");
    assert_log_message(log_level::info, "baz");
    assert_log_empty();
}

test_case(postfix_increment)
{
    mock_virtual_container cnt;
    cnt.add_file("foo");

    auto it = cnt.begin();
    auto jt = it++;

    assert_ne(cnt.begin(), cnt.end());
    assert_ne(it, jt);
    assert_eq(it, cnt.end());
    assert_eq(jt, cnt.begin());
}

test_case(deref_out_of_bounds)
{
    mock_virtual_container cnt;
    auto it = cnt.begin();

    assert_throws_logged(*it);
    assert_log_message(log_level::error, "virtual container iterator out of bounds");
    assert_log_empty();
}

test_case(pointer_out_of_bounds)
{
    mock_virtual_container cnt;
    auto it = cnt.begin();

    assert_throws_logged(it->open());
    assert_log_message(log_level::error, "virtual container iterator out of bounds");
    assert_log_empty();
}

end_suite(virtual_container_iterator_test);
