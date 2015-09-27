NAME:=script-test
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/test \
	libs/cog/script \

SOURCES:= \
	function_verb_test.cpp \
	id_test.cpp \
	message_table_test.cpp \
	message_type_test.cpp \
	mock_verb_test.cpp \
	string_table_test.cpp \
	symbol_table_test.cpp \
	type_test.cpp \
	value_test.cpp \
	verb_table_test.cpp \
	verb_traits_test.cpp \
