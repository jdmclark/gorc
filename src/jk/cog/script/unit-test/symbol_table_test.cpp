#include "jk/cog/script/symbol_table.hpp"
#include "test/test.hpp"

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

    auto bar_si = st.get_symbol_index("bar");
    assert_eq(std::get<0>(bar_si), symbol_scope::local_symbol);
    assert_eq(std::get<1>(bar_si), 1U);

    auto bar_sym = st.get_symbol_by_index(symbol_scope::local_symbol, size_t(1));
    auto bar_sym2 = st.get_symbol("bar");

    assert_eq(bar_sym, bar_sym2);
}

test_case(redefinition)
{
    symbol_table st;

    st.add_symbol("foo");

    assert_log_empty();

    auto foo_si = st.get_symbol_index("foo");
    assert_eq(std::get<0>(foo_si), symbol_scope::local_symbol);
    assert_eq(std::get<1>(foo_si), 0U);

    st.add_symbol("foo");

    assert_log_message(log_level::warning, "symbol 'foo' redefinition");
    assert_log_empty();

    auto foo_gsi = st.get_symbol_index("foo");
    assert_eq(std::get<0>(foo_gsi), symbol_scope::local_symbol);
    assert_eq(std::get<1>(foo_gsi), 1U);

    assert_eq(std::get<1>(st.get_symbol_by_index(symbol_scope::local_symbol, size_t(0)))->name,
              std::string("foo"));
}

test_case(shadow_global)
{
    symbol_table st;

    auto g0 = st.get_existing_symbol("global13");
    assert_eq(std::get<0>(g0.get_value()), symbol_scope::global_symbol);
    assert_eq(std::get<1>(g0.get_value())->sequence_number, 13U);

    assert_log_empty();
    st.add_symbol("global13");

    assert_log_message(log_level::warning, "symbol 'global13' shadows global symbol");
    assert_log_empty();

    auto g1 = st.get_existing_symbol("global13");
    assert_eq(std::get<0>(g1.get_value()), symbol_scope::local_symbol);
    assert_eq(std::get<1>(g1.get_value())->sequence_number, 0U);
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
