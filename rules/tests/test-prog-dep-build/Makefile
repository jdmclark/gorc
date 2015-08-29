include ../../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output

generate-output:
	-@$(MAKE) -f $(RULES)/rules.mk all \
		-C mock-src \
		PROG_NAME_OVERRIDE="mock-prog.mk" \
		BIN=$(abspath $(TESTSUITE_DIR)/bin) \
		SRC=$(abspath .) \
		> $(TESTSUITE_DIR)/temp; \
		echo "exit code: $$?" >> $(RAW_OUTPUT)
	cat $(TESTSUITE_DIR)/temp >> $(RAW_OUTPUT)
	find $(TESTSUITE_DIR)/bin -type f  \
		| LC_ALL="C" sort >> $(RAW_OUTPUT)
