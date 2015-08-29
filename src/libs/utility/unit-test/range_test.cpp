#include "utility/range.hpp"
#include "test/test.hpp"

#include <vector>

begin_suite(range_test);

test_case(ctor)
{
    std::vector<int> input { 0, 1, 2, 3, 4 };
    std::vector<int> output;

    gorc::range<std::vector<int>::iterator> rng(input.begin(),
                                                input.end());
    for(auto const &e : rng) {
        output.emplace_back(e);
    }

    assert_range_eq(output, std::vector<int>({ 0, 1, 2, 3, 4 }));
}

test_case(make_range_iterators)
{
    std::vector<int> input { 0, 1, 2, 3, 4 };
    std::vector<int> output;

    for(auto const &e : gorc::make_range(input.begin(), input.end())) {
        output.emplace_back(e);
    }

    assert_range_eq(output, std::vector<int>({ 0, 1, 2, 3, 4 }));
}

test_case(make_range_tuple)
{
    std::vector<int> input { 0, 1, 2, 3, 4 };
    std::vector<int> output;

    auto rng_tuple = std::make_tuple(input.begin(), input.end());
    for(auto const &e : gorc::make_range(rng_tuple)) {
        output.emplace_back(e);
    }

    assert_range_eq(output, std::vector<int>({ 0, 1, 2, 3, 4 }));
}

test_case(make_range_pair)
{
    std::vector<int> input { 0, 1, 2, 3, 4 };
    std::vector<int> output;

    auto rng_tuple = std::make_pair(input.begin(), input.end());
    for(auto const &e : gorc::make_range(rng_tuple)) {
        output.emplace_back(e);
    }

    assert_range_eq(output, std::vector<int>({ 0, 1, 2, 3, 4 }));
}

test_case(make_range_object)
{
    std::vector<int> input { 0, 1, 2, 3, 4 };
    std::vector<int> output;

    for(auto const &e : gorc::make_range(input)) {
        output.emplace_back(e);
    }

    assert_range_eq(output, std::vector<int>({ 0, 1, 2, 3, 4 }));
}

test_case(range_empty)
{
    std::vector<int> empty;
    std::vector<int> full { 0, 1, 2, 3, 4 };

    assert_true(gorc::make_range(empty).empty());
    assert_true(!gorc::make_range(full).empty());
}

end_suite(range_test);
