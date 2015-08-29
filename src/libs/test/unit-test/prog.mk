NAME:=test-test
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/test \

SOURCES:= \
	mock_reporter.cpp \
	test_assert_test.cpp \
	test_case_order.cpp \
	test_expect_test.cpp \
	test_framework_test.cpp \
	test_strings_test.cpp \
