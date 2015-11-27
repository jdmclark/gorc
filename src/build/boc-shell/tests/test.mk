include ../../../../../../rules/test.mk

BOC_SHELL:=$(BUILD_BIN)/boc-shell
BOC_SHELL_OPTS:=

run-input-boc:
	$(BOC_SHELL) $(BOC_SHELL_OPTS) --script input.boc >> $(RAW_OUTPUT) 2>&1 ; true

test: \
	run-input-boc \
	process-raw-output \
	compare-output
