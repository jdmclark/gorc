NAME:=vfs-test
BIN_DIR=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/vfs \
	libs/test \

SOURCES:= \
	episode_entry_type_test.cpp \
	virtual_container_iterator_test.cpp \
