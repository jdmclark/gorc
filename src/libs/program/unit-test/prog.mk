NAME:=program-test
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/program \
	libs/test \

SOURCES:= \
	options_test.cpp \

LIBRARIES:=$(SYSTEM_LIBRARIES)
