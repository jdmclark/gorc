#include "test/test.hpp"
#include "jk/cog/script/symbol_table.hpp"

using namespace gorc;
using namespace gorc::cog;

begin_suite(symbol_table_test);

test_case(basic)
{
    symbol_table st;

    assert_eq(st.size(), size_t(0));
    assert_eq(st.begin(), st.end());

    st.add_symbol("foo");
    st.add_symbol("bar");
    st.add_symbol("baz");

    assert_eq(st.size(), size_t(3));

    auto it = st.begin();

    assert_eq(it->name, std::string("foo"));
    ++it;
    assert_eq(it->name, std::string("bar"));
    ++it;
    assert_eq(it->name, std::string("baz"));
    ++it;
    assert_eq(it, st.end());

    assert_eq(st.get_symbol_index("bar"), size_t(1));

    auto const& bar_sym = st.get_symbol_by_index(size_t(1));
    auto const& bar_sym2 = st.get_symbol("bar");

    assert_eq(&bar_sym, &bar_sym2);
}

test_case(redefinition)
{
    symbol_table st;

    st.add_symbol("foo");

    assert_log_empty();
    assert_eq(st.get_symbol_index("foo"), size_t(0));

    st.add_symbol("foo");

    assert_log_message(log_level::warning, "symbol 'foo' redefinition");
    assert_log_empty();
    assert_eq(st.get_symbol_index("foo"), size_t(1));

    assert_eq(st.get_symbol_by_index(size_t(0)).name, std::string("foo"));
}

test_case(undefined)
{
    symbol_table st;

    assert_log_empty();

    st.get_symbol("foo");

    assert_log_message(log_level::warning, "symbol 'foo' used without being defined");
    assert_log_empty();

    assert_eq(st.size(), size_t(1));
}

end_suite(symbol_table_test);
