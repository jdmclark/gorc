#include "test/test.hpp"
#include "utility/zip.hpp"

using namespace gorc;

begin_suite(zip_test);

test_case(empty)
{
    assert_true(zip().empty());
}

test_case(non_empty)
{
    std::vector<int> values { 0, 1 };
    assert_true(!zip(values).empty());
}

test_case(zip_iterator_begin)
{
    std::vector<int> a { 0, 1, 2 };
    std::vector<int> b { 3, 4, 5 };
    std::vector<int> c { 6, 7, 8 };

    auto zi = make_zip_iterator(a.begin(), b.begin(), c.begin());

    assert_eq(std::get<0>(*zi), 0);
    assert_eq(std::get<1>(*zi), 3);
    assert_eq(std::get<2>(*zi), 6);
}

test_case(single)
{
    std::vector<int> const values { 0, 1, 2, 3, 4, 5, 6 };
    std::vector<int> zipped;

    for(auto const &value : zip(values)) {
        zipped.push_back(std::get<0>(value));
    }

    assert_eq(values, zipped);
}

test_case(multiple)
{
    std::vector<int> a { 0, 1, 2 };
    std::vector<int> b { 3, 4, 5 };
    std::vector<int> c { 6, 7, 8 };
    std::vector<int> zipped;

    for(auto const &value : zip(a, b, c)) {
        zipped.push_back(std::get<0>(value));
        zipped.push_back(std::get<1>(value));
        zipped.push_back(std::get<2>(value));
    }

    std::vector<int> expected { 0, 3, 6, 1, 4, 7, 2, 5, 8 };

    assert_eq(expected, zipped);
}

test_case(uneven)
{
    std::vector<int> a { 2, 3, 4, 5 };
    std::vector<int> b { 6, 7, 8 };
    std::vector<int> c { 9, 10 };

    std::vector<int> zipped;

    for(auto const &value : zip(a, b, c)) {
        zipped.push_back(std::get<0>(value));
        zipped.push_back(std::get<1>(value));
        zipped.push_back(std::get<2>(value));
    }

    std::vector<int> expected { 2, 6, 9, 3, 7, 10 };

    assert_eq(expected, zipped);
}

test_case(write_back)
{
    std::vector<int> a { 0, 1, 2, 3, 4 };
    std::vector<int> b { 5, 6, 7, 8, 9 };

    for(auto const &value : zip(a, b)) {
        std::get<0>(value) += std::get<1>(value);
    }

    assert_eq(a, (std::vector<int> { 5, 7, 9, 11, 13 }));
    assert_eq(b, (std::vector<int> { 5, 6, 7, 8, 9 }));
}

end_suite(zip_test);
