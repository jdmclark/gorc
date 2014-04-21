# Creates a simple executable which runs stdout through a std_output_stream.

include ../../test.mk

testsuite: \
	run-testsuite \
	process-raw-output \
	compare-output
	
run-testsuite:
	$(ITEST_BIN_PATH)/output_stream > $(RAW_OUTPUT) \
		; echo "==== Testsuite returns: $$? ====" >> $(RAW_OUTPUT)
