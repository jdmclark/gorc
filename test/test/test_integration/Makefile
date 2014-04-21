# Creates and runs a unit test suite with various assertion failures.

include ../../test.mk

testsuite: \
	run-testsuite \
	process-raw-output \
	compare-output
	
run-testsuite:
	$(ITEST_BIN_PATH)/test_integration > $(RAW_OUTPUT) \
		; echo "==== Testsuite returns: $$? ====" >> $(RAW_OUTPUT)
