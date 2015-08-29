# Helpers for makefile test cases run under run-test.

ifndef TEST_MK_INCLUDED
TEST_MK_INCLUDED:=1

# First rule: forces the default target to run under an instance of run-testsuite.
default-testsuite-target: real-default-testsuite-target

include $(dir $(lastword $(MAKEFILE_LIST)))flags.mk
export FAILED_TEST_LOG

real-default-testsuite-target:
	@$(MAKE) -f $(RULES)/testsuite.mk test ALLOWED_TEST_NAMES="."

# The test target should only be run under run-test
test: assert-under-run-test
ifndef INSIDE_PROJECT_TEST_RUNNER
assert-under-run-test:
	$(error "Cannot run test target manually")
else
assert-under-run-test:
endif

# Miscellaneous test rules

# Console word wrap causes output instability. Disable it.
export GORC_NO_WORD_WRAP=1

RAW_OUTPUT:=$(TESTSUITE_DIR)/raw-output.txt
CURRENT_OUTPUT:=$(TESTSUITE_DIR)/current-output.txt
WORKING_OUTPUT:=$(TESTSUITE_DIR)/temp-output.txt
EXPECTED_OUTPUT:=expected-output.txt
PLATFORM_EXPECTED_OUTPUT:=expected-output-$(PLATFORM).txt

process-raw-output:
	# Strip PWD from output
	-@sed \
		-e 's?c:/?/c/?Ig' \
		-e 's?c:\\?/c/?Ig' \
		-e 's?\\?/?g' \
		-e 's?$(abspath .)?$$TESTDIR$$?g' \
		-e 's?$(PROJECT_ROOT)?$$ROOT$$?g' \
		-e 's?$(PLATFORM)?$$PLATFORM$$?g' \
		-e 's?\.EXE??Ig' \
		$(foreach reg,$(EXTRA_REGEX),-e $(reg)) \
		$(RAW_OUTPUT) > $(CURRENT_OUTPUT)

compare-output:
	if [ -e $(PLATFORM_EXPECTED_OUTPUT) ]; then \
		diff -u -w $(PLATFORM_EXPECTED_OUTPUT) $(CURRENT_OUTPUT); \
	else \
		diff -u -w $(EXPECTED_OUTPUT) $(CURRENT_OUTPUT); \
	fi

endif
