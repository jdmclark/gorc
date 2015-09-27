include ../../../../../rules/test.mk

COG:=$(BIN)/cog

test: \
	run-cog \
	process-raw-output \
	compare-output

run-cog:
	-$(COG) input.cog >>$(RAW_OUTPUT) 2>&1
