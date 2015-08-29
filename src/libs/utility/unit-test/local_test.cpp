#include "test/test.hpp"
#include "utility/local.hpp"
#include <thread>
#include <vector>
#include <string>
#include <mutex>

namespace {
    std::vector<std::string> messages;
}

class mock_local : public gorc::local {
    template <typename LocalT> friend class gorc::local_factory;
private:
    mock_local()
    {
        messages.push_back("mock_local constructor called");
    }

public:
    ~mock_local()
    {
        messages.push_back("mock_local destructor called");
    }
};

begin_suite(local_test);

test_case(local_destructor_called)
{
    auto thread_proc = []() {
        gorc::get_local<mock_local>();
    };

    std::thread th(thread_proc);
    th.join();

    assert_eq(messages,
              (std::vector<std::string> { "mock_local constructor called",
                                          "mock_local destructor called" }));
}

end_suite(local_test);
