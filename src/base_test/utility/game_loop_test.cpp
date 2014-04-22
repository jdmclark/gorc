#include "test/test.h"
#include "base/utility/game_loop.h"

namespace game_loop_test_mocks {

class mock_timestamp_factory : public gorc::utility::timestamp_factory {
public:
    uint32_t prev_time = 0;
    uint32_t time_delta = 0;

    virtual gorc::utility::timestamp make_timestamp() override {
        return gorc::timestamp(prev_time += time_delta);
    }
};

class mock_game_loop : public gorc::utility::game_loop {
public:
    int update_called;
    int draw_called;
    int halt_count;

    mock_game_loop(mock_timestamp_factory& tf, uint32_t ms_per_update, int halt_count)
        : gorc::utility::game_loop(tf, ms_per_update), update_called(0), draw_called(0), halt_count(halt_count) {
        return;
    }

    virtual void update(gorc::time) override {
        ++update_called;
        if(update_called == halt_count) {
            stop();
        }
    }

    virtual void draw(gorc::time) override {
        ++draw_called;
    }
};

}

using namespace game_loop_test_mocks;

begin_suite(game_loop_test);

test_case(draw_every_cycle) {
    mock_timestamp_factory tf;
    tf.time_delta = 1;

    mock_game_loop loop(tf, 5, 5);
    loop.run();

    assert_eq(loop.update_called, 5);
    assert_eq(loop.draw_called, 25);
}

end_suite(game_loop_test);
