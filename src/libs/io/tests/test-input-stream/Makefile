# Creates a simple executable which runs stdin through a std_input_stream.

include ../test.mk

test: \
	run-testsuite \
	process-raw-output \
	compare-output

run-testsuite:
	echo -n "Hello, World!" | $(TEST_BIN)/example-input-stream > $(RAW_OUTPUT) \
		; echo "==== Testsuite returns: $$? ====" >> $(RAW_OUTPUT)
