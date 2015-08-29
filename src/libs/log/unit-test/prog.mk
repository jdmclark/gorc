NAME:=log-test
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/log \
	libs/test \

SOURCES:= \
	diagnostic_context_test.cpp \
	log_level_test.cpp \
