NAME:=io-test
BIN_DIR=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/io \
	libs/test \

SOURCES:= \
	input_stream_test.cpp \
	memory_file_test.cpp \
	output_stream_test.cpp \
