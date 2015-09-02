include ../../../../../../rules/test.mk

COGCHECK:=$(BIN)/cogcheck
COGCHECK_OPTS:=

test: \
	run-cogcheck \
	process-raw-output \
	compare-output

run-cogcheck:
	-$(COGCHECK) $(COGCHECK_OPTS) input.cog 1>>$(RAW_OUTPUT) 2>>$(RAW_OUTPUT)
