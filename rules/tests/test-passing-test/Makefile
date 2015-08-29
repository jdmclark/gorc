include ../../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output

generate-output:
	-@$(MAKE) -f $(RULES)/testsuite.mk test \
		ALLOWED_TEST_NAMES="mock-test" \
		FAILED_TEST_LOG=$(abspath $(TESTSUITE_DIR)/mock-failed-test-log.txt) \
		> $(TESTSUITE_DIR)/temp; \
		echo "exit code: $$?" >> $(RAW_OUTPUT)
	cat $(TESTSUITE_DIR)/temp \
		| grep -vi make \
		| grep -v "^$$" \
		| sed 's/(..\..s)/(XX.Xs)/' >> $(RAW_OUTPUT)
