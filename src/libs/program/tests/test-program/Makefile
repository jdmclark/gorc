# Exercises program features.

include ../test.mk

PROGRAM=$(TEST_BIN)/example-program

test: \
	run-no-args \
	run-message \
	run-throw-std \
	run-throw-unk \
	run-diagnostics-stdout \
	run-diagnostics \
	process-raw-output \
	compare-output

run-no-args:
	echo "==== no args ====" >> $(RAW_OUTPUT)
	$(PROGRAM) >> $(RAW_OUTPUT)

run-message:
	echo "==== message ====" >> $(RAW_OUTPUT)
	$(PROGRAM) --message "Hello, World!" >> $(RAW_OUTPUT)

run-throw-std:
	echo "==== throw std ====" >> $(RAW_OUTPUT)
	$(PROGRAM) --throw-std-exception 2>&1 | grep ".*" >> $(RAW_OUTPUT)

run-throw-unk:
	echo "==== throw unk ====" >> $(RAW_OUTPUT)
	$(PROGRAM) --throw-unk-exception 2>&1 | grep ".*" >> $(RAW_OUTPUT)

run-diagnostics-stdout:
	echo "==== diagnostics not on stdout ====" >> $(RAW_OUTPUT)
	$(PROGRAM) --message "this is on stdout" \
		--info-message "this is informational" \
		1> $(TESTSUITE_DIR)/diag-stdout.txt \
		2> $(TESTSUITE_DIR)/diag-stderr.txt
	echo "---- stdout ----" >> $(RAW_OUTPUT)
	cat $(TESTSUITE_DIR)/diag-stdout.txt >> $(RAW_OUTPUT)
	echo "---- stderr ----" >> $(RAW_OUTPUT)
	cat $(TESTSUITE_DIR)/diag-stderr.txt >> $(RAW_OUTPUT)

run-diagnostics:
	echo "==== diagnostics ====" >> $(RAW_OUTPUT)
	$(PROGRAM) --info-message "info message" \
		--warning-message "warning message" \
		--error-message "error message" 2>&1 | grep ".*" >> $(RAW_OUTPUT)
