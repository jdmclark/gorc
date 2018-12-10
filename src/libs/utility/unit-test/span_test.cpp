#include "test/test.hpp"
#include "utility/span.hpp"
#include <algorithm>

begin_suite(span_test);

test_case(simple_array_test)
{
    int my_array[5] = {1, 2, 3, 4, 5};

    auto s = gorc::make_span(my_array);

    std::vector<int> expected{1, 2, 3, 4, 5};
    assert_range_eq(expected, s);
}

test_case(simple_array_size_test)
{
    int my_array[5] = {1, 2, 3, 4, 5};

    auto s = gorc::make_span(static_cast<int *>(my_array), 4);

    std::vector<int> expected{1, 2, 3, 4};
    assert_range_eq(expected, s);
}

test_case(simple_sequential_container_test)
{
    std::vector<int> my_array{1, 2, 3, 4, 5};

    auto s = gorc::make_span(my_array);

    std::vector<int> expected{1, 2, 3, 4, 5};
    assert_range_eq(expected, s);
}

test_case(transform_array_test)
{
    int my_array[5] = {1, 2, 3, 4, 5};

    auto s = gorc::make_span(my_array);
    for(auto &em : s) {
        em *= 2;
    }

    std::vector<int> expected{2, 4, 6, 8, 10};
    assert_range_eq(expected, s);
}

test_case(subspan_test)
{
    int my_array[8] = {1, 2, 3, 4, 5, 6, 7, 8};

    auto s = gorc::make_span(my_array);

    std::vector<int> expected{3, 4, 5, 6};
    assert_range_eq(expected, s.subspan(2, 4));
}

test_case(subspan_starts_past_end)
{
    int my_array[4] = {1, 2, 3, 4};
    auto s = gorc::make_span(my_array);

    assert_throws(s.subspan(6, 2), std::logic_error, "span::subspan starts past end of span");
}

test_case(subspan_exceeds_size)
{
    int my_array[4] = {1, 2, 3, 4};
    auto s = gorc::make_span(my_array);

    assert_throws(s.subspan(2, 5), std::logic_error, "span::subspan exceeds dimensions of span");
}

test_case(subspan_to_end)
{
    int my_array[8] = {1, 2, 3, 4, 5, 6, 7, 8};

    auto s = gorc::make_span(my_array);

    std::vector<int> expected{5, 6, 7, 8};
    assert_range_eq(expected, s.subspan(4, gorc::span_to_end));
}

test_case(subspan_empty_test)
{
    int my_array[8] = {1, 2, 3, 4, 5, 6, 7, 8};

    auto s = gorc::make_span(my_array);

    std::vector<int> expected{};
    assert_range_eq(expected, s.subspan(0, 0));
    assert_range_eq(expected, s.subspan(8, 0));
}

test_case(size_correct)
{
    int my_array[3] = {1, 2, 3};

    auto s = gorc::make_span(my_array);
    assert_eq(s.size(), 3U);
}

test_case(empty_correct)
{
    int my_array[3] = {1, 2, 3};

    auto s = gorc::make_span(my_array);
    assert_true(!s.empty());

    auto s1 = s.subspan(2, 0);
    assert_true(s1.empty());
}

test_case(size_bytes_correct)
{
    int my_array[3] = {1, 2, 3};

    auto s = gorc::make_span(my_array);
    assert_eq(s.size_bytes(), 12U);
}

test_case(byte_span_correct)
{
    uint16_t my_array[4] = {0xABCDU, 0x1234U, 0xBADFU, 0x8958U};

    auto s = gorc::make_span(my_array);
    auto byte_s = s.as_bytes();

    std::vector<uint16_t> other;
    other.resize(4);

    auto t = gorc::make_span(reinterpret_cast<char *>(other.data()), 8U); // NOLINT

    std::copy(std::begin(byte_s), std::end(byte_s), std::begin(t));

    std::vector<uint16_t> expected{0xABCDU, 0x1234U, 0xBADFU, 0x8958U};
    assert_range_eq(expected, other);
}

test_case(as_bytes_const)
{
    uint16_t my_array[4] = {0xABCDU, 0x1234U, 0xBADFU, 0x8958U};

    auto const s = gorc::make_span(my_array);
    gorc::span<char const> byte_s = s.as_bytes();

    std::vector<uint16_t> other;
    other.resize(4);

    auto t = gorc::make_span(reinterpret_cast<char *>(other.data()), 8U); // NOLINT

    std::copy(std::begin(byte_s), std::end(byte_s), std::begin(t));

    std::vector<uint16_t> expected{0xABCDU, 0x1234U, 0xBADFU, 0x8958U};
    assert_range_eq(expected, other);
}

test_case(unsigned_byte_span_correct)
{
    uint16_t my_array[4] = {0xABCDU, 0x1234U, 0xBADFU, 0x8958U};

    auto s = gorc::make_span(my_array);
    auto byte_s = s.as_unsigned_bytes();

    std::vector<uint16_t> other;
    other.resize(4);

    auto t = gorc::make_span(reinterpret_cast<unsigned char *>(other.data()), 8U); // NOLINT

    std::copy(std::begin(byte_s), std::end(byte_s), std::begin(t));

    std::vector<uint16_t> expected{0xABCDU, 0x1234U, 0xBADFU, 0x8958U};
    assert_range_eq(expected, other);
}

test_case(as_unsigned_bytes_const)
{
    uint16_t my_array[4] = {0xABCDU, 0x1234U, 0xBADFU, 0x8958U};

    auto const s = gorc::make_span(my_array);
    gorc::span<unsigned char const> byte_s = s.as_unsigned_bytes();

    std::vector<uint16_t> other;
    other.resize(4);

    auto t = gorc::make_span(reinterpret_cast<char *>(other.data()), 8U); // NOLINT

    std::copy(std::begin(byte_s), std::end(byte_s), std::begin(t));

    std::vector<uint16_t> expected{0xABCDU, 0x1234U, 0xBADFU, 0x8958U};
    assert_range_eq(expected, other);
}

test_case(as_unsigned_const_bytes)
{
    uint16_t my_array[4] = {0xABCDU, 0x1234U, 0xBADFU, 0x8958U};

    auto const s = gorc::make_span(my_array);
    gorc::span<unsigned char const> byte_s = s.as_unsigned_const_bytes();

    std::vector<uint16_t> other;
    other.resize(4);

    auto t = gorc::make_span(reinterpret_cast<char *>(other.data()), 8U); // NOLINT

    std::copy(std::begin(byte_s), std::end(byte_s), std::begin(t));

    std::vector<uint16_t> expected{0xABCDU, 0x1234U, 0xBADFU, 0x8958U};
    assert_range_eq(expected, other);
}

test_case(as_const_bytes)
{
    uint16_t const my_array[4] = {0xABCDU, 0x1234U, 0xBADFU, 0x8958U};

    auto byte_s = gorc::make_span(my_array).as_const_bytes();

    std::vector<uint16_t> other;
    other.resize(4);

    auto t = gorc::make_span(reinterpret_cast<char *>(other.data()), 8U); // NOLINT

    std::copy(std::begin(byte_s), std::end(byte_s), std::begin(t));

    std::vector<uint16_t> expected{0xABCDU, 0x1234U, 0xBADFU, 0x8958U};
    assert_range_eq(expected, other);
}

test_case(string_span_correct)
{
    auto s = gorc::make_string_span("Hello, World!");
    assert_eq(s.size(), 13U);

    std::string expected("Hello, World!");
    assert_range_eq(s, expected);
}

test_case(implicit_cast_to_const)
{
    std::vector<int> my_array{1, 2, 3, 4, 5};
    auto s = gorc::make_span(my_array);

    auto l = [](gorc::span<int const> s) {
        std::vector<int> rv;
        std::copy(s.begin(), s.end(), std::back_inserter(rv));
        return rv;
    };

    auto result = l(s);
}

test_case(span_iterator_constructor)
{
    std::vector<int> my_array{1, 2, 3, 4, 5};
    auto s = gorc::make_span(my_array);

    auto it = s.begin();
    assert_eq(*it, my_array.at(0));

    auto jt = it;
    assert_eq(*jt, my_array.at(0));
}

test_case(span_iterator_dereference)
{
    std::vector<std::string> my_array{"hello", "world"};
    auto s = gorc::make_span(my_array);

    auto it = s.begin();
    assert_eq(it->front(), 'h');
}

test_case(span_iterator_pre_increment)
{
    std::vector<int> my_array{1, 2, 3};
    auto s = gorc::make_span(my_array);

    auto it = s.begin();
    auto jt = ++it;

    assert_eq(*it, 2);
    assert_eq(*jt, 2);
}

test_case(span_iterator_post_increment)
{
    std::vector<int> my_array{1, 2, 3};
    auto s = gorc::make_span(my_array);

    auto it = s.begin();
    auto jt = it++;

    assert_eq(*it, 2);
    assert_eq(*jt, 1);
}

test_case(span_iterator_pre_decrement)
{
    std::vector<int> my_array{1, 2, 3};
    auto s = gorc::make_span(my_array);

    auto it = s.begin() + 2;
    auto jt = --it;

    assert_eq(*it, 2);
    assert_eq(*jt, 2);
}

test_case(span_iterator_post_decrement)
{
    std::vector<int> my_array{1, 2, 3};
    auto s = gorc::make_span(my_array);

    auto it = s.begin() + 2;
    auto jt = it--;

    assert_eq(*it, 2);
    assert_eq(*jt, 3);
}

test_case(span_iterator_add)
{
    std::vector<int> my_array{1, 2, 3};
    auto s = gorc::make_span(my_array);

    auto it = s.begin() + 2;
    assert_eq(*it, 3);
}

test_case(span_iterator_add_assign)
{
    std::vector<int> my_array{1, 2, 3};
    auto s = gorc::make_span(my_array);

    auto it = s.begin();
    it += 2;
    assert_eq(*it, 3);
}

test_case(span_iterator_sub)
{
    std::vector<int> my_array{1, 2, 3};
    auto s = gorc::make_span(my_array);

    auto it = s.end() - 3;
    assert_eq(*it, 1);
}

test_case(span_iterator_sub_assign)
{
    std::vector<int> my_array{1, 2, 3};
    auto s = gorc::make_span(my_array);

    auto it = s.end();
    it -= 2;
    assert_eq(*it, 2);
}

test_case(span_iterator_difference)
{
    std::vector<int> my_array{1, 2, 3};
    std::vector<int> other_array{4, 5};

    auto s = gorc::make_span(my_array);
    auto t = gorc::make_span(other_array);

    auto dist = s.end() - s.begin();
    assert_eq(dist, 3U);

    assert_throws((t.end() - s.begin()),
                  std::logic_error,
                  "span_iterator computed difference from multiple spans");
}

test_case(span_iterator_equality)
{
    std::vector<int> my_array{1, 2, 3};
    std::vector<int> other_array{4, 5};

    auto s = gorc::make_span(my_array);
    auto t = gorc::make_span(other_array);

    assert_true(s.begin() == s.begin());
    assert_true(!(s.begin() == (s.begin() + 1)));
    assert_throws(
        (s.begin() == t.begin()), std::logic_error, "span_iterator compared multiple spans");
}

test_case(span_iterator_inequality)
{
    std::vector<int> my_array{1, 2, 3};
    std::vector<int> other_array{4, 5};

    auto s = gorc::make_span(my_array);
    auto t = gorc::make_span(other_array);

    assert_true(!(s.begin() != s.begin()));
    assert_true(s.begin() != (s.begin() + 1));
    assert_throws(
        (s.begin() != t.begin()), std::logic_error, "span_iterator compared multiple spans");
}

test_case(span_iterator_less)
{
    std::vector<int> my_array{1, 2, 3};
    auto s = gorc::make_span(my_array);

    auto a = s.begin();
    auto b = s.begin() + 1;
    auto c = s.begin() + 1;
    auto d = s.begin() + 2;

    assert_true(a < b);
    assert_true(!(b < a));
    assert_true(!(b < c));
    assert_true(c < d);
}

test_case(span_iterator_less_eq)
{
    std::vector<int> my_array{1, 2, 3};
    auto s = gorc::make_span(my_array);

    auto a = s.begin();
    auto b = s.begin() + 1;
    auto c = s.begin() + 1;
    auto d = s.begin() + 2;

    assert_true(a <= b);
    assert_true(!(b <= a));
    assert_true(b <= c);
    assert_true(c <= d);
}

test_case(span_iterator_greater)
{
    std::vector<int> my_array{1, 2, 3};
    auto s = gorc::make_span(my_array);

    auto a = s.begin();
    auto b = s.begin() + 1;
    auto c = s.begin() + 1;
    auto d = s.begin() + 2;

    assert_true(b > a);
    assert_true(!(a > b));
    assert_true(!(c > b));
    assert_true(d > c);
}

test_case(span_iterator_greater_eq)
{
    std::vector<int> my_array{1, 2, 3};
    auto s = gorc::make_span(my_array);

    auto a = s.begin();
    auto b = s.begin() + 1;
    auto c = s.begin() + 1;
    auto d = s.begin() + 2;

    assert_true(b >= a);
    assert_true(!(a > b));
    assert_true(c >= b);
    assert_true(d >= c);
}

test_case(begin_end)
{
    std::vector<int> my_array{1, 2, 3};
    gorc::span<int> s = gorc::make_span(my_array);

    auto it = s.begin();
    assert_eq(*it, 1);
    it += 3;
    assert_eq(it, s.end());
}

test_case(begin_end_const)
{
    std::vector<int> my_array{1, 2, 3};
    gorc::span<int> const s = gorc::make_span(my_array);

    auto it = s.begin();
    assert_eq(*it, 1);
    it += 3;
    assert_eq(it, s.end());
}

test_case(cbegin_cend)
{
    std::vector<int> my_array{1, 2, 3};
    gorc::span<int> s = gorc::make_span(my_array);

    auto it = s.cbegin();
    assert_eq(*it, 1);
    it += 3;
    assert_eq(it, s.cend());
}

test_case(data_correct)
{
    std::vector<int> my_array{1, 2, 3};
    auto s = gorc::make_span(my_array);

    assert_eq(s.data(), my_array.data());
}

test_case(data_const_correct)
{
    std::vector<int> my_array{1, 2, 3};
    gorc::span<int> const s = gorc::make_span(my_array);

    assert_eq(s.data(), my_array.data());
}

end_suite(span_test);
