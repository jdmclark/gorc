include ../../../../../rules/test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output

generate-output:
	$(TEST_BIN)/example-file-log $(TESTSUITE_DIR)/logfile.txt
	cat $(TESTSUITE_DIR)/logfile.txt > $(RAW_OUTPUT)
