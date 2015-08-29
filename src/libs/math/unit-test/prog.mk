NAME:=math-test
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/math \
	libs/test \

SOURCES:= \
	almost_equal_test.cpp \
	vector_test.cpp \
