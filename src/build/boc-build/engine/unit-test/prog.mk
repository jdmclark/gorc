NAME:=boc-build-engine-test
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	build/boc-build/engine \
	libs/test \

SOURCES:= \
	entity_closure_test.cpp \
	mock_entity.cpp \

LIBRARIES:=$(SYSTEM_LIBRARIES)
