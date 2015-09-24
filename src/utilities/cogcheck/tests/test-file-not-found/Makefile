include ../../../../../rules/test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output

generate-output:
	-$(BIN)/cogcheck input.cog >> $(RAW_OUTPUT) 2>&1
