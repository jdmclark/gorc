# Creates a simple executable which tests various errors in native_file.

include ../../test.mk

testsuite: \
	setup-files \
	run-testsuite \
	cleanup-files \
	process-raw-output \
	compare-output

setup-files:
	echo "Some existing content" > $(TESTSUITE_DIR)/existing.txt
	touch $(TESTSUITE_DIR)/write_denied.txt
	chmod a-w $(TESTSUITE_DIR)/write_denied.txt
	echo "small" > $(TESTSUITE_DIR)/small.txt
	echo "somewhat larger" > $(TESTSUITE_DIR)/medium.txt

cleanup-files:
	cat $(TESTSUITE_DIR)/existing.txt >> $(RAW_OUTPUT)
	chmod a+w $(TESTSUITE_DIR)/write_denied.txt
	
run-testsuite:
	$(ITEST_BIN_PATH)/native_file > $(RAW_OUTPUT) \
		; echo "==== Testsuite returns: $$? ====" >> $(RAW_OUTPUT)
