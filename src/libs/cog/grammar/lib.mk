NAME:=grammar

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/log \
	libs/cog/ast \
	libs/text \

SOURCES:= \
	grammar.cpp \
	lexer.cpp \
	parser.cpp \
