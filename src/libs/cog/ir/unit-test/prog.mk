NAME:=ir-test
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/test \
	libs/cog/ir \

SOURCES:= \
	ir_printer_test.cpp \
