#include <nullunit/nullunit.h>
#include "cog/symbols/table.h"
#include "cog/symbols/exception.h"

using namespace gorc::cog::symbols;
using namespace gorc::cog::vm;

BeginSuite(SymTable_Test);

Case(Exists) {
	symbol_table table;
}

Case(add_symbol) {
	symbol_table table;

	table.add_symbol(symbol_type::integer, "foo", value(10),
		true, "desc", 0, 0, true);
}

Case(GetSymbolByIndex) {
	symbol_table table;

	table.add_symbol(symbol_type::integer, "foo", value(10),
		true, "desc", 0, 0, true);
	table.add_symbol(symbol_type::integer, "bar", value(20),
		true, "desc", 0, 0, true);
	table.add_symbol(symbol_type::integer, "baz", value(30),
		true, "desc", 0, 0, true);

	const symbol& s = table.get_symbol(0);
	Test_Expect_Eq(s.name, "foo");
	Test_Expect_Eq(static_cast<int>(s.default_value), 10);

	const symbol& t = table.get_symbol(1);
	Test_Expect_Eq(t.name, "bar");
	Test_Expect_Eq(static_cast<int>(t.default_value), 20);

	const symbol& v = table.get_symbol(2);
	Test_Expect_Eq(v.name, "baz");
	Test_Expect_Eq(static_cast<int>(v.default_value), 30);
}

Case(GetSymbolByName) {
	symbol_table table;

	table.add_symbol(symbol_type::integer, "foo", value(10),
		true, "desc", 0, 0, true);
	table.add_symbol(symbol_type::integer, "bar", value(20),
		true, "desc", 0, 0, true);
	table.add_symbol(symbol_type::integer, "baz", value(30),
		true, "desc", 0, 0, true);

	const symbol& s = table.get_symbol("foo");
	Test_Expect_Eq(s.name, "foo");
	Test_Expect_Eq(static_cast<int>(s.default_value), 10);

	const symbol& t = table.get_symbol("bar");
	Test_Expect_Eq(t.name, "bar");
	Test_Expect_Eq(static_cast<int>(t.default_value), 20);

	const symbol& v = table.get_symbol("baz");
	Test_Expect_Eq(v.name, "baz");
	Test_Expect_Eq(static_cast<int>(v.default_value), 30);
}

Case(SymbolRedefinition) {
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

	Test_Assert_Always("Did not throw redefinition exception");
}

Case(SymbolUndefinedName) {
	symbol_table table;

	table.add_symbol(symbol_type::integer, "foo", value(10),
		true, "desc", 0, 0, true);

	try {
		const symbol& s = table.get_symbol("bar");
	}
	catch(const undefined_symbol_exception&) {
		return;
	}

	Test_Assert_Always("Did not throw undefined exception");
}

Case(SymbolUndefinedIndex) {
	symbol_table table;

	table.add_symbol(symbol_type::integer, "foo", value(10),
		true, "desc", 0, 0, true);

	try {
		const symbol& s = table.get_symbol(1);
	}
	catch(const undefined_symbol_exception&) {
		return;
	}

	Test_Assert_Always("Did not throw undefined exception");
}

Case(IsSymbolDefinedName) {
	symbol_table table;

	Test_Assert(!table.is_symbol_defined("foo"));

	table.add_symbol(symbol_type::integer, "foo", value(10),
		true, "desc", 0, 0, true);

	Test_Assert(table.is_symbol_defined("foo"));

	Test_Assert(!table.is_symbol_defined("bar"));
}

Case(IsSymbolDefinedIndex) {
	symbol_table table;

	Test_Assert(!table.is_symbol_defined(0));
	Test_Assert(!table.is_symbol_defined(1));

	table.add_symbol(symbol_type::integer, "foo", value(10),
		true, "desc", 0, 0, true);

	Test_Assert(table.is_symbol_defined(0));
	Test_Assert(!table.is_symbol_defined(1));
}

EndSuite(SymTable_Test);
