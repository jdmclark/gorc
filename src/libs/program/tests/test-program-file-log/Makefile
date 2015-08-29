include ../test.mk

PROGRAM=$(TEST_BIN)/example-program

test: \
	generate-log-file \
	generate-output \
	process-raw-output \
	compare-output

generate-log-file:
	echo "==== standard output ====" >> $(RAW_OUTPUT)
	GORC_LOG_FILE=$(TESTSUITE_DIR)/program_log.txt \
		$(PROGRAM) \
		--message "Hello, World!" \
		--info-message "this is informational" \
		--warning-message "warning message" \
		--error-message "error message" \
		2>&1 | grep ".*" >> $(RAW_OUTPUT)

generate-output:
	echo "==== log output ====" >> $(RAW_OUTPUT)
	cat $(TESTSUITE_DIR)/program_log.txt >> $(RAW_OUTPUT)
