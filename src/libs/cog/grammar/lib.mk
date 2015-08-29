NAME:=grammar

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/log \
	libs/io \
	libs/cog/ast \

ifndef GRAMMAR_CODEGEN_RULES_DEFINED
GRAMMAR_CODEGEN_RULES_DEFINED:=1

$(MODULE_SRC)/lexer.cpp: $(MODULE_SRC)/lexer.l $(MODULE_SRC)/parser.cpp
	flex --outfile=$@ $<

$(MODULE_SRC)/parser.cpp: $(MODULE_SRC)/parser.y
	bison --output=$@ $<
endif

SOURCES:= \
	grammar.cpp \
	lexer.cpp \
	parser.cpp \
