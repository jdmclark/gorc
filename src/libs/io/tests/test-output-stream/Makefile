# Creates a simple executable which runs stdout through a std_output_stream.

include ../test.mk

test: \
	run-testsuite \
	process-raw-output \
	compare-output
	
run-testsuite:
	$(TEST_BIN)/example-output-stream > $(RAW_OUTPUT) \
		; echo "==== Testsuite returns: $$? ====" >> $(RAW_OUTPUT)
