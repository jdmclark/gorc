NAME:=ast-test
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/test \
	libs/cog/ast \

SOURCES:= \
	factory_test.cpp \
	node_test.cpp \
