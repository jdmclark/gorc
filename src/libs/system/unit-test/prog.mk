NAME:=system-test
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/test \
	libs/system \

SOURCES:= \
	env_test.cpp \
	pipe_test.cpp \
	self_test.cpp \
