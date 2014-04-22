#include "test/test.h"
#include "base/utility/event_bus.h"
#include "base/utility/place_controller.h"

namespace place_controller_test_mocks {

int test1_place_constructor;
int test1_place_destructor;
int test1_place_start;
int test1_place_can_stop;
int test1_place_stop;
int test2_place_constructor;
int test2_place_start;
int test2_place_can_stop;
int test2_place_stop;

class test1_place {
public:
    test1_place(int a, int b, int c) {
        assert_eq(a, 1);
        assert_eq(b, 5);
        assert_eq(c, 7);
    }
};

class test2_place {
public:
    test2_place(int a, int b) {
        assert_eq(a, 12);
        assert_eq(b, 96);
    }
};

class test1_presenter : public gorc::presenter {
public:
    test1_presenter() {
        ++test1_place_constructor;
    }

    ~test1_presenter() {
        ++test1_place_destructor;
    }

    virtual bool can_stop() const override {
        ++test1_place_can_stop;
        return gorc::presenter::can_stop();
    }

    virtual void start(gorc::event_bus&) override {
        ++test1_place_start;
    }

    virtual void stop() override {
        ++test1_place_stop;
        gorc::presenter::stop();
    }
};

class test2_presenter : public gorc::presenter {
public:
    test2_presenter() {
        ++test2_place_constructor;
    }

    virtual bool can_stop() const override {
        ++test2_place_can_stop;
        return false;
    }

    virtual void start(gorc::event_bus&) override {
        ++test2_place_start;
    }

    virtual void stop() override {
        ++test2_place_stop;
        return;
    }
};

class test_presenter_mapper {
public:
    std::unique_ptr<test1_presenter> get_presenter(std::unique_ptr<test1_place>) const {
        return gorc::make_unique<test1_presenter>();
    }

    std::unique_ptr<test2_presenter> get_presenter(std::unique_ptr<test2_place>) const {
        return gorc::make_unique<test2_presenter>();
    }
};

}

begin_suite(place_controller_test);

test_case(go_to_test1_then_test2) {
    place_controller_test_mocks::test1_place_constructor = 0;
    place_controller_test_mocks::test1_place_destructor = 0;
    place_controller_test_mocks::test1_place_start = 0;
    place_controller_test_mocks::test1_place_can_stop = 0;
    place_controller_test_mocks::test1_place_stop = 0;
    place_controller_test_mocks::test2_place_constructor = 0;
    place_controller_test_mocks::test2_place_can_stop = 0;
    place_controller_test_mocks::test2_place_stop = 0;
    place_controller_test_mocks::test2_place_start = 0;

    gorc::event_bus bus;
    place_controller_test_mocks::test_presenter_mapper mapper;
    gorc::place_controller<place_controller_test_mocks::test_presenter_mapper> pc(bus, mapper);

    assert_eq(place_controller_test_mocks::test1_place_constructor, 0);

    pc.go_to<place_controller_test_mocks::test1_place>(1, 5, 7);

    assert_eq(place_controller_test_mocks::test1_place_constructor, 1);
    assert_eq(place_controller_test_mocks::test1_place_start, 1);
    assert_eq(place_controller_test_mocks::test2_place_constructor, 0);

    pc.go_to<place_controller_test_mocks::test2_place>(12, 96);

    assert_eq(place_controller_test_mocks::test1_place_can_stop, 1);
    assert_eq(place_controller_test_mocks::test1_place_stop, 1);
    assert_eq(place_controller_test_mocks::test1_place_destructor, 1);
    assert_eq(place_controller_test_mocks::test2_place_constructor, 1);
    assert_eq(place_controller_test_mocks::test2_place_start, 1);
    assert_eq(place_controller_test_mocks::test2_place_can_stop, 0);

    pc.go_to<place_controller_test_mocks::test1_place>(1, 5, 7);

    assert_eq(place_controller_test_mocks::test2_place_can_stop, 1);
    assert_eq(place_controller_test_mocks::test2_place_stop, 0);

    assert_eq(place_controller_test_mocks::test1_place_constructor, 1);
}

end_suite(place_controller_test);
