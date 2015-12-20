# Provides the global test target

ifndef TESTSUITE_INCLUDED
TESTSUITE_INCLUDED:=1

# First rule: forces the full testsuite to run
test:

include $(dir $(lastword $(MAKEFILE_LIST)))paths.mk
export FAILED_TEST_LOG

export BOC_SHELL:=$(BUILD_BIN)/boc-shell

test:
	@echo "Running tests:"
	-@rm -f $(FAILED_TEST_LOG)
	@$(MAKE) -k -s --ignore-errors --no-print-directory \
		-f $(RULES)/testsuite.mk full-run-testsuite \
		2>/dev/null
	@if [ -e $(FAILED_TEST_LOG) ]; then \
		echo; \
		echo "Failed tests:"; \
		echo; \
		cat $(FAILED_TEST_LOG); \
		exit 1; \
	else \
		echo "**** SUITE PASSED ****"; \
	fi

# Search for all tests from the current directory
ALLOWED_TEST_NAMES:= \
	"test-*" \
	"unit-test*" \

ALL_TESTS:= \
	$(shell find . -path ./pkg -prune -o \( \
		$(foreach name,$(strip $(ALLOWED_TEST_NAMES)), -type d -name $(name) -o) -false \
		\) -print )

.PHONY: $(ALL_TESTS)
$(ALL_TESTS):
	$(SCRIPTS)/run-test $(patsubst %/,%,$@)

.PHONY: full-run-testsuite
full-run-testsuite: $(ALL_TESTS)

endif
