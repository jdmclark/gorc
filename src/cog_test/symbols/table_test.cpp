#include "test/test.h"
#include "cog/symbols/table.h"
#include "cog/symbols/exception.h"

using namespace gorc::cog::symbols;
using namespace gorc::cog::vm;

begin_suite(SymTable_Test);

test_case(Exists) {
    symbol_table table;
}

test_case(add_symbol) {
    symbol_table table;

    table.add_symbol(symbol_type::integer, "foo", value(10),
        true, "desc", 0, 0, true);
}

test_case(GetSymbolByIndex) {
    symbol_table table;

    table.add_symbol(symbol_type::integer, "foo", value(10),
        true, "desc", 0, 0, true);
    table.add_symbol(symbol_type::integer, "bar", value(20),
        true, "desc", 0, 0, true);
    table.add_symbol(symbol_type::integer, "baz", value(30),
        true, "desc", 0, 0, true);

    const symbol& s = table.get_symbol(0);
    expect_eq(s.name, "foo");
    expect_eq(static_cast<int>(s.default_value), 10);

    const symbol& t = table.get_symbol(1);
    expect_eq(t.name, "bar");
    expect_eq(static_cast<int>(t.default_value), 20);

    const symbol& v = table.get_symbol(2);
    expect_eq(v.name, "baz");
    expect_eq(static_cast<int>(v.default_value), 30);
}

test_case(GetSymbolByName) {
    symbol_table table;

    table.add_symbol(symbol_type::integer, "foo", value(10),
        true, "desc", 0, 0, true);
    table.add_symbol(symbol_type::integer, "bar", value(20),
        true, "desc", 0, 0, true);
    table.add_symbol(symbol_type::integer, "baz", value(30),
        true, "desc", 0, 0, true);

    const symbol& s = table.get_symbol("foo");
    expect_eq(s.name, "foo");
    expect_eq(static_cast<int>(s.default_value), 10);

    const symbol& t = table.get_symbol("bar");
    expect_eq(t.name, "bar");
    expect_eq(static_cast<int>(t.default_value), 20);

    const symbol& v = table.get_symbol("baz");
    expect_eq(v.name, "baz");
    expect_eq(static_cast<int>(v.default_value), 30);
}

test_case(SymbolRedefinition) {
    symbol_table table;

    table.add_symbol(symbol_type::integer, "foo", value(10),
        true, "desc", 0, 0, true);

    try {
        table.add_symbol(symbol_type::integer, "foo", value(20),
            true, "desc", 0, 0, true);
    }
    catch(const symbol_redefinition_exception&) {
        return;
    }

    assert_always("Did not throw redefinition exception");
}

test_case(SymbolUndefinedName) {
    symbol_table table;

    table.add_symbol(symbol_type::integer, "foo", value(10),
        true, "desc", 0, 0, true);

    try {
        const symbol& s = table.get_symbol("bar");
    }
    catch(const undefined_symbol_exception&) {
        return;
    }

    assert_always("Did not throw undefined exception");
}

test_case(SymbolUndefinedIndex) {
    symbol_table table;

    table.add_symbol(symbol_type::integer, "foo", value(10),
        true, "desc", 0, 0, true);

    try {
        const symbol& s = table.get_symbol(1);
    }
    catch(const undefined_symbol_exception&) {
        return;
    }

    assert_always("Did not throw undefined exception");
}

test_case(IsSymbolDefinedName) {
    symbol_table table;

    assert_true(!table.is_symbol_defined("foo"));

    table.add_symbol(symbol_type::integer, "foo", value(10),
        true, "desc", 0, 0, true);

    assert_true(table.is_symbol_defined("foo"));

    assert_true(!table.is_symbol_defined("bar"));
}

test_case(IsSymbolDefinedIndex) {
    symbol_table table;

    assert_true(!table.is_symbol_defined(0));
    assert_true(!table.is_symbol_defined(1));

    table.add_symbol(symbol_type::integer, "foo", value(10),
        true, "desc", 0, 0, true);

    assert_true(table.is_symbol_defined(0));
    assert_true(!table.is_symbol_defined(1));
}

end_suite(SymTable_Test);
