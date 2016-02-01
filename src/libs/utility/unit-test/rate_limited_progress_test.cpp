#include <chrono>
#include <thread>
#include "test/test.hpp"
#include "utility/rate_limited_progress.hpp"
#include "log/log.hpp"

using namespace gorc;

namespace {

    class mock_progress : public progress {
    public:
        virtual void advance(size_t steps_to_advance) override
        {
            LOG_INFO(format("advancing %d steps") % steps_to_advance);
        }

        virtual void finished() override
        {
            LOG_INFO("finished");
        }
    };

}

begin_suite(rate_limited_progress_test);

test_case(finishes_fast)
{
    mock_progress mp;
    rate_limited_progress rlp(mp, std::chrono::seconds(500));

    rlp.advance(1);
    rlp.advance(1);
    rlp.advance(1);
    rlp.advance(1);

    assert_log_empty();

    rlp.finished();

    assert_log_message(log_level::info, "advancing 4 steps");
    assert_log_message(log_level::info, "finished");
    assert_log_empty();
}

test_case(finishes_at_end)
{
    mock_progress mp;
    rate_limited_progress rlp(mp, std::chrono::seconds(2));

    rlp.advance(1);
    rlp.advance(1);

    assert_log_empty();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    assert_log_empty();

    rlp.advance(1);

    assert_log_message(log_level::info, "advancing 3 steps");
    assert_log_empty();

    rlp.finished();

    assert_log_message(log_level::info, "finished");
    assert_log_empty();
}

end_suite(rate_limited_progress_test);
