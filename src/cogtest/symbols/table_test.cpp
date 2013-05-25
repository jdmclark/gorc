#include <nullunit/nullunit.h>
#include "cog/symbols/table.h"
#include "cog/symbols/exception.h"

using namespace Gorc::Cog::Symbols;
using namespace Gorc::Cog::VM;

BeginSuite(SymTable_Test);

Case(Exists) {
	SymbolTable table;
}

Case(AddSymbol) {
	SymbolTable table;

	table.AddSymbol(SymbolType::Int, "foo", Value(10),
		true, "desc", 0, 0, true);
}

Case(GetSymbolByIndex) {
	SymbolTable table;

	table.AddSymbol(SymbolType::Int, "foo", Value(10),
		true, "desc", 0, 0, true);
	table.AddSymbol(SymbolType::Int, "bar", Value(20),
		true, "desc", 0, 0, true);
	table.AddSymbol(SymbolType::Int, "baz", Value(30),
		true, "desc", 0, 0, true);

	const Symbol& s = table.GetSymbol(0);
	Test_Expect_Eq(s.Name, "foo");
	Test_Expect_Eq(static_cast<int>(s.DefaultValue), 10);

	const Symbol& t = table.GetSymbol(1);
	Test_Expect_Eq(t.Name, "bar");
	Test_Expect_Eq(static_cast<int>(t.DefaultValue), 20);

	const Symbol& v = table.GetSymbol(2);
	Test_Expect_Eq(v.Name, "baz");
	Test_Expect_Eq(static_cast<int>(v.DefaultValue), 30);
}

Case(GetSymbolByName) {
	SymbolTable table;

	table.AddSymbol(SymbolType::Int, "foo", Value(10),
		true, "desc", 0, 0, true);
	table.AddSymbol(SymbolType::Int, "bar", Value(20),
		true, "desc", 0, 0, true);
	table.AddSymbol(SymbolType::Int, "baz", Value(30),
		true, "desc", 0, 0, true);

	const Symbol& s = table.GetSymbol("foo");
	Test_Expect_Eq(s.Name, "foo");
	Test_Expect_Eq(static_cast<int>(s.DefaultValue), 10);

	const Symbol& t = table.GetSymbol("bar");
	Test_Expect_Eq(t.Name, "bar");
	Test_Expect_Eq(static_cast<int>(t.DefaultValue), 20);

	const Symbol& v = table.GetSymbol("baz");
	Test_Expect_Eq(v.Name, "baz");
	Test_Expect_Eq(static_cast<int>(v.DefaultValue), 30);
}

Case(SymbolRedefinition) {
	SymbolTable table;

	table.AddSymbol(SymbolType::Int, "foo", Value(10),
		true, "desc", 0, 0, true);

	try {
		table.AddSymbol(SymbolType::Int, "foo", Value(20),
			true, "desc", 0, 0, true);
	}
	catch(const SymbolRedefinitionException&) {
		return;
	}

	Test_Assert_Always("Did not throw redefinition exception");
}

Case(SymbolUndefinedName) {
	SymbolTable table;

	table.AddSymbol(SymbolType::Int, "foo", Value(10),
		true, "desc", 0, 0, true);

	try {
		const Symbol& s = table.GetSymbol("bar");
	}
	catch(const UndefinedSymbolException&) {
		return;
	}

	Test_Assert_Always("Did not throw undefined exception");
}

Case(SymbolUndefinedIndex) {
	SymbolTable table;

	table.AddSymbol(SymbolType::Int, "foo", Value(10),
		true, "desc", 0, 0, true);

	try {
		const Symbol& s = table.GetSymbol(1);
	}
	catch(const UndefinedSymbolException&) {
		return;
	}

	Test_Assert_Always("Did not throw undefined exception");
}

Case(IsSymbolDefinedName) {
	SymbolTable table;

	Test_Assert(!table.IsSymbolDefined("foo"));

	table.AddSymbol(SymbolType::Int, "foo", Value(10),
		true, "desc", 0, 0, true);

	Test_Assert(table.IsSymbolDefined("foo"));

	Test_Assert(!table.IsSymbolDefined("bar"));
}

Case(IsSymbolDefinedIndex) {
	SymbolTable table;

	Test_Assert(!table.IsSymbolDefined(0));
	Test_Assert(!table.IsSymbolDefined(1));

	table.AddSymbol(SymbolType::Int, "foo", Value(10),
		true, "desc", 0, 0, true);

	Test_Assert(table.IsSymbolDefined(0));
	Test_Assert(!table.IsSymbolDefined(1));
}

EndSuite(SymTable_Test);
