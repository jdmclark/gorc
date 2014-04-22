#include "test/test.h"
#include "base/utility/pool.h"

using gorc::pool;

class pool_test_element {
public:
    int value;
    int ctor_called;

    pool_test_element() : value(0), ctor_called(38) {
        return;
    }

    pool_test_element(int value) : value(value), ctor_called(58) {
        return;
    }

    pool_test_element(int a, int b, int c) : value(a + b + c), ctor_called(78) {
        return;
    }
};

begin_suite(pool_test);

test_case(count_empty) {
    pool<pool_test_element> p;

    int ct = 0;
    for(auto& em : p) {
        ++ct;
    }

    assert_eq(ct, 0);
}

test_case(emplace_default_ctor) {
    pool<pool_test_element, 4> p;
    auto& em = p.emplace();

    assert_eq(em.value, 0);
    assert_eq(em.ctor_called, 38);
    assert_eq(em.get_id(), 0);
}

test_case(emplace_sv_ctor) {
    pool<pool_test_element, 4> p;
    auto& em = p.emplace(5);

    assert_eq(em.value, 5);
    assert_eq(em.ctor_called, 58);
    assert_eq(em.get_id(), 0);
}

test_case(emplace_mv_ctor) {
    pool<pool_test_element, 4> p;
    auto& em = p.emplace(1, 3, 7);

    assert_eq(em.value, 11);
    assert_eq(em.ctor_called, 78);
    assert_eq(em.get_id(), 0);
}

test_case(grow_gorces) {
    pool<pool_test_element, 4> p;

    for(int i = 0; i < 95; ++i) {
        p.emplace(i);
    }

    int j = 0;
    for(auto& em : p) {
        assert_eq(em.value, j);
        ++j;
    }

    assert_eq(j, 95);
}

test_case(set_elements) {
    pool<pool_test_element, 4> p;

    for(int i = 0; i < 95; ++i) {
        p.emplace(0);
    }

    int j = 0;
    for(auto& em : p) {
        em.value = j;
        ++j;
    }

    j = 0;
    for(auto& em : p) {
        assert_eq(em.value, j);
        ++j;
    }
}

test_case(remove_elements) {
    pool<pool_test_element, 4> p;

    for(int i = 0; i < 95; ++i) {
        p.emplace(i);
    }

    int j = 0;
    for(auto& em : p) {
        if((em.value % 7) == 0) {
            p.erase(em);
        }

        ++j;
    }

    assert_eq(j, 95);

    j = 0;
    for(auto& em : p) {
        assert_ne((em.value % 7), 0);
        ++j;
    }

    assert_eq(j, 81);
}

test_case(remove_then_insert) {
    pool<pool_test_element, 4> p;

    for(int i = 0; i < 95; ++i) {
        p.emplace(i);
    }

    auto& em = p[15];
    p.erase(em);

    int j = 0;
    for(auto& em : p) {
        assert_ne(em.get_id(), 15);
        ++j;
    }

    assert_eq(j, 94);

    auto& new_em = p.emplace();
    assert_eq(new_em.get_id(), 15);
}

test_case(const_iterator_to_deleted) {
    pool<pool_test_element, 4> p;

    for(int i = 0; i < 6; ++i) {
        p.emplace(i);
    }

    p.erase(p[0]);

    const auto& q = p;

    int ct = 0;
    for(const auto& em : q) {
        ++ct;
    }

    assert_eq(ct, 5);
}

end_suite(pool_test);
