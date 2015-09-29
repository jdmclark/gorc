NAME:=vm-test
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/test \
	libs/cog/vm \

SOURCES:= \
	heap_test.cpp \
	sleep_record_test.cpp \
	virtual_machine_test.cpp \
