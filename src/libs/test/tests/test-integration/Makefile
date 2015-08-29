include ../../../../../rules/test.mk

test: \
	run-testsuite \
	process-raw-output \
	compare-output
	
run-testsuite:
	$(TEST_BIN)/example-test-suite > $(RAW_OUTPUT) \
		; echo "==== Testsuite returns: $$? ====" >> $(RAW_OUTPUT)
