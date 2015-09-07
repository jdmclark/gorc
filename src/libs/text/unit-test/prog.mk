NAME:=text-test
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/test \
	libs/text \

SOURCES:= \
	tokenizer_test.cpp \
