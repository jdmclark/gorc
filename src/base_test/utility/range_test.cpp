#include "test/test.h"
#include "base/utility/range.h"
#include <vector>
#include <utility>
#include <tuple>

using namespace gorc;

begin_suite(range_test);

test_case(rng) {
    std::vector<int> s;
    for(int i = 0; i < 68; ++i) {
        s.push_back(i);
    }

    int sum = 0;
    for(auto v : make_range(s.begin(), s.end())) {
        sum += v;
    }

    assert_eq(sum, 2278);
}

test_case(assign_rng) {
    std::vector<int> s;
    for(int i = 0; i < 68; ++i) {
        s.push_back(0);
    }

    int i = 0;
    for(auto& v : make_range(s.begin(), s.end())) {
        v = i++;
    }

    int sum = 0;
    for(auto v : make_range(s.begin(), s.end())) {
        sum += v;
    }

    assert_eq(sum, 2278);
}

test_case(pair_rng) {
    std::vector<int> s;
    for(int i = 0; i < 68; ++i) {
        s.push_back(i);
    }

    auto p = make_pair(s.begin(), s.end());

    int sum = 0;
    for(auto v : make_range(p)) {
        sum += v;
    }

    assert_eq(sum, 2278);
}

test_case(tuple_rng) {
    std::vector<int> s;
    for(int i = 0; i < 68; ++i) {
        s.push_back(i);
    }

    auto p = make_tuple(s.begin(), s.end());

    int sum = 0;
    for(auto v : make_range(p)) {
        sum += v;
    }

    assert_eq(sum, 2278);
}

test_case(rng_from_rng) {
    std::vector<int> s;
    for(int i = 0; i < 68; ++i) {
        s.push_back(i);
    }

    auto p = make_range(s);

    int sum = 0;
    for(auto v : p) {
        sum += v;
    }

    assert_eq(sum, 2278);
}

end_suite(range_test);
