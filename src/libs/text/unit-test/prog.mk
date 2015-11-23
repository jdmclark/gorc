NAME:=text-test
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/test \
	libs/text \

SOURCES:= \
	generic_tokenizer_test.cpp \
	token_helpers_test.cpp \
	json_serialization_test.cpp \
	json_specification_test.cpp \
	json_tokenizer_test.cpp \
