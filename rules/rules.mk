# Specifies the default 'all' target

ifndef RULES_INCLUDED
RULES_INCLUDED:=1

.DEFAULT: all
.PHONY: all
all: bin

# Include paths.mk from current directory
include $(dir $(lastword $(MAKEFILE_LIST)))paths.mk

include $(RULES)/functions.mk
include $(RULES)/flags.mk

# Force install git hooks
$(shell cp $(SCRIPTS)/pre-commit $(PROJECT_ROOT)/.git/hooks/pre-commit)

.PHONY: bin
bin:
	@$(MAKE) -f $(RULES)/build.mk build-all-bin-modules

.PHONY: test
test:
	@$(MAKE) -f $(RULES)/testsuite.mk test

.PHONY: test-coverage-report
test-coverage-report:
	$(MAKE) -f $(RULES)/build.mk generate-test-coverage-report

.PHONY: coveralls-test-coverage-report
coveralls-test-coverage-report:
	$(MAKE) -f $(RULES)/build.mk generate-coveralls-test-coverage-report

endif
