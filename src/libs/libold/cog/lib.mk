NAME:=cog

DEPENDENCIES:= \
	libs/base \
	libs/log \

ifndef COG_CODEGEN_RULES_DEFINED
COG_CODEGEN_RULES_DEFINED:=1
$(MODULE_SRC)/grammar/lexer.cpp: $(MODULE_SRC)/grammar/lexer.lex $(MODULE_SRC)/grammar/parser.cpp
	flex --outfile=$@ $<

$(MODULE_SRC)/grammar/parser.cpp: $(MODULE_SRC)/grammar/parser.y
	bison --output=$@ $<
endif

SOURCES:= \
	ast/factory.cpp \
	ast/node.cpp \
	compiler.cpp \
	constants.cpp \
	grammar/instance.cpp \
	grammar/lexer.cpp \
	grammar/parser.cpp \
	instance.cpp \
	ir/code_printer.cpp \
	ir/printer.cpp \
	ir/text_printer.cpp \
	script.cpp \
	scripts/script_continuation.cpp \
	scripts/script_model.cpp \
	scripts/script_presenter.cpp \
	stages/1_generate_ast/generate_ast.cpp \
	stages/2_semantic_analysis/code_visitor.cpp \
	stages/2_semantic_analysis/expression_visitor.cpp \
	stages/2_semantic_analysis/label_visitor.cpp \
	stages/2_semantic_analysis/lval_visitor.cpp \
	stages/2_semantic_analysis/semantic_analysis.cpp \
	stages/2_semantic_analysis/symbolext_visitor.cpp \
	stages/2_semantic_analysis/symbolfield_visitor.cpp \
	stages/2_semantic_analysis/symbol_visitor.cpp \
	stages/4_generate_code/code_generation.cpp \
	stages/4_generate_code/code_visitor.cpp \
	stages/4_generate_code/expression_visitor.cpp \
	stages/4_generate_code/lval_expression_visitor.cpp \
	stages/4_generate_code/nonval_expression_visitor.cpp \
	symbols/symbol.cpp \
	symbols/table.cpp \
	verbs/table.cpp \
	verbs/verb.cpp \
	verbs/verb_id.cpp \
	vm/code_buffer.cpp \
	vm/code_buffer_read_stream.cpp \
	vm/code_buffer_write_stream.cpp \
	vm/exception.cpp \
	vm/jump_table.cpp \
	vm/value.cpp \
	vm/virtual_machine.cpp \
