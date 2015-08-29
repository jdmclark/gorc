NAME:=example-input-stream
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/io \

SOURCES:= \
	test.cpp \

LIBRARIES:=$(SYSTEM_LIBRARIES)
