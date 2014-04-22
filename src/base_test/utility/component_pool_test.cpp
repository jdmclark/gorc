#include "test/test.h"
#include "base/utility/component_pool.h"

using namespace gorc;

class test_component {
public:
    int value;

    test_component(int value = 0) : value(value) {
        return;
    }
};

begin_suite(component_pool_test);

test_case(empty_pool) {
    component_pool<test_component> p;

    for(auto& em : p) {
        assert_always("expected empty, contains an element");
    }
}

test_case(empty_range) {
    component_pool<test_component> p;

    for(int i = 0; i < 20; ++i) {
        if(i == 5) {
            continue;
        }

        p.emplace(entity_id(i));
    }

    for(auto& em : p.find(entity_id(5))) {
        assert_always("expected empty range, found an element");
    }

    for(auto& em : p) {
        assert_ne(std::get<0>(em), entity_id(5));
    }
}

test_case(iterate_all) {
    component_pool<test_component> p;

    for(int i = 0; i < 20; ++i) {
        p.emplace(entity_id(i), i + 100);
    }

    for(auto& em : p) {
        assert_eq(std::get<0>(em) + 100, std::get<1>(em).value);
    }
}

test_case(update_range) {
    component_pool<test_component> p;

    for(int i = 0; i < 20; ++i) {
        p.emplace(entity_id(i), i + 100);
    }

    for(int i = 0; i < 10; ++i) {
        p.emplace(entity_id(5), i + 500);
    }

    bool seen_one = false;
    for(auto& em : p.find(entity_id(5))) {
        if(std::get<1>(em).value < 500) {
            seen_one = true;
            std::get<1>(em).value = 511;
        }
    }

    assert_true(seen_one);

    for(auto& em : p.find(entity_id(5))) {
        assert_ge(std::get<1>(em).value, 500);
    }
}

test_case(erase_range) {
    component_pool<test_component> p;

    for(int i = 0; i < 20; ++i) {
        p.emplace(entity_id(i), 7);
    }

    for(int i = 0; i < 5; ++i) {
        p.emplace(entity_id(5), 2);
    }

    int slow_ct = 0;
    for(auto& em : p) {
        if(std::get<0>(em) == entity_id(5)) {
            ++slow_ct;
        }
    }

    assert_eq(slow_ct, 6);

    int fast_ct = 0;
    for(auto& em : p.find(entity_id(5))) {
        ++fast_ct;
    }

    assert_eq(fast_ct, 6);

    p.erase(p.find(entity_id(5)));

    slow_ct = 0;
    for(auto& em : p) {
        if(std::get<0>(em) == entity_id(5)) {
            ++slow_ct;
        }

        assert_eq(std::get<1>(em).value, 7);
    }
}

test_case(erase_one) {
    component_pool<test_component> p;

    for(int i = 0; i < 5; ++i) {
        p.emplace(entity_id(0), i);
    }

    auto rng = p.find(entity_id(0));

    for(auto it = rng.begin(); it != rng.end();) {
        if(it->second.value == 3) {
            auto it_bak = it++;
            p.erase(it_bak);
        }
        else {
            ++it;
        }
    }

    int ct = 0;
    for(auto& em : p) {
        expect_ne(em.second.value, 3);
        ++ct;
    }

    assert_eq(ct, 4);
}

end_suite(component_pool_test);
