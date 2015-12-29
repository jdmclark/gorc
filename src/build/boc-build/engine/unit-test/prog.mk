NAME:=boc-build-engine-test
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	build/boc-build/engine \
	libs/test \

SOURCES:= \
	adjacency_list_test.cpp \
	dirty_list_test.cpp \
	entity_allocator_test.cpp \
	entity_closure_test.cpp \
	entity_registry_test.cpp \
	entity_scheduler_test.cpp \
	entity_serialization_test.cpp \
	mock_entity.cpp \

LIBRARIES:=$(SYSTEM_LIBRARIES)
