NAME:=grammar

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/log \
	libs/cog/ast \
	libs/text \

SOURCES:= \
	cog_sub_tokenizer_state_machine.cpp \
	cog_tokenizer_state_machine.cpp \
	grammar.cpp \
	lexer.cpp \
	parser.cpp \
