#include "test/test.h"
#include <memory>
#include <fstream>

#include "cog_test/language/syntactic_test_fixture.h"

class FunctionalSyntacticTestFixture : public SyntacticTestFixture {
public:
    FunctionalSyntacticTestFixture() : SyntacticTestFixture("test/cog/functional/syntactic") {
        return;
    }
};

begin_suite_fixture(FunctionalSyntacticTest, FunctionalSyntacticTestFixture);

test_case(s2_array) {
    ParseFile("array.cog");
    AssertResult(0, 0);
}

test_case(s2_comment_hash_code) {
    ParseFile("comment_hash_code.cog");
    AssertResult(0, 0);
}

test_case(s2_comment_hash_symbol) {
    ParseFile("comment_hash_symbol.cog");
    AssertResult(0, 0);
}

test_case(s2_comment_slash_code) {
    ParseFile("comment_slash_code.cog");
    AssertResult(0, 0);
}

test_case(s2_comment_slash_symbol) {
    ParseFile("comment_slash_symbol.cog");
    AssertResult(0, 0);
}

test_case(s2_do) {
    ParseFile("do.cog");
    AssertResult(0, 0);
}

test_case(s2_empty) {
    ParseFile("empty.cog");
    AssertResult(0, 0);
}

test_case(s2_expr_ops) {
    ParseFile("expr_ops.cog");
    AssertResult(0, 0);
}

test_case(s2_for) {
    ParseFile("for.cog");
    AssertResult(0, 0);
}

test_case(s2_for_combined) {
    ParseFile("for_combined.cog");
    AssertResult(0, 0);
}

test_case(s2_for_expr) {
    ParseFile("for_expr.cog");
    AssertResult(0, 0);
}

test_case(s2_for_inc) {
    ParseFile("for_inc.cog");
    AssertResult(0, 0);
}

test_case(s2_for_itdecl) {
    ParseFile("for_itdecl.cog");
    AssertResult(0, 0);
}

test_case(s2_if) {
    ParseFile("if.cog");
    AssertResult(0, 0);
}

test_case(s2_ifelse) {
    ParseFile("ifelse.cog");
    AssertResult(0, 0);
}

test_case(s2_ifelseif) {
    ParseFile("ifelseif.cog");
    AssertResult(0, 0);
}

test_case(s2_ifelseif_ambiguous) {
    ParseFile("ifelseif_ambiguous.cog");
    AssertResult(0, 0);
}

test_case(s2_label_degen) {
    ParseFile("label_degen.cog");
    AssertResult(1, 0);
}

test_case(s2_label_multi) {
    ParseFile("label_multi.cog");
    AssertResult(0, 0);
}

test_case(s2_label_none) {
    ParseFile("label_none.cog");
    AssertResult(0, 0);
}

test_case(s2_label_one) {
    ParseFile("label_one.cog");
    AssertResult(0, 0);
}

test_case(s2_label_redefinition) {
    ParseFile("label_redefinition.cog");
    AssertResult(0, 0);
}

test_case(s2_literal_float) {
    ParseFile("literal_float.cog");
    AssertResult(0, 0);
}

test_case(s2_literal_int) {
    ParseFile("literal_int.cog");
    AssertResult(0, 0);
}

test_case(s2_literal_int_hex) {
    ParseFile("literal_int_hex.cog");
    AssertResult(0, 0);
}

test_case(s2_stmt_break) {
    ParseFile("stmt_break.cog");
    AssertResult(0, 0);
}

test_case(s2_stmt_call) {
    ParseFile("stmt_call.cog");
    AssertResult(0, 0);
}

test_case(s2_stmt_return) {
    ParseFile("stmt_return.cog");
    AssertResult(0, 0);
}

test_case(s2_stmt_stop) {
    ParseFile("stmt_stop.cog");
    AssertResult(0, 0);
}

test_case(s2_symbol_basic) {
    ParseFile("symbol_basic.cog");
    AssertResult(0, 0);
}

test_case(s2_symbol_empty) {
    ParseFile("symbol_empty.cog");
    AssertResult(0, 0);
}

test_case(s2_symbol_ext) {
    ParseFile("symbol_ext.cog");
    AssertResult(0, 0);
}

test_case(s2_symbol_filename) {
    ParseFile("symbol_filename.cog");
    AssertResult(0, 0);
}

test_case(s2_symbol_flex) {
    ParseFile("symbol_flex.cog");
    AssertResult(0, 0);
}

test_case(s2_symbol_hexint) {
    ParseFile("symbol_hexint.cog");
    AssertResult(0, 0);
}

test_case(s2_symbol_int) {
    ParseFile("symbol_int.cog");
    AssertResult(0, 0);
}

test_case(s2_symbol_template) {
    ParseFile("symbol_template.cog");
    AssertResult(0, 0);
}

test_case(s2_while) {
    ParseFile("while.cog");
    AssertResult(0, 0);
}

end_suite(FunctionalSyntacticTest);
