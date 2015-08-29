# Creates a simple executable which runs stdin through a binary std_input_stream.

include ../test.mk

test: \
	run-testsuite \
	process-raw-output \
	compare-output

run-testsuite:
	echo -n 'Hello,\r\nWorld!' | $(TEST_BIN)/example-binary-stream > $(TESTSUITE_DIR)/temp.txt
	tr ['\r'] [X] <$(TESTSUITE_DIR)/temp.txt >$(RAW_OUTPUT)
