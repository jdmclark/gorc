include ../../../../../rules/test.mk

COG:=$(BIN)/cog

run-cog: \
	run-single-cog \
	process-raw-output \
	compare-output

run-scenario: \
	run-single-scenario \
	process-raw-output \
	compare-output

run-single-cog:
	-$(COG) --scenario ../default.scn >>$(RAW_OUTPUT) 2>&1

run-single-scenario:
	-$(COG) --scenario scenario.scn >>$(RAW_OUTPUT) 2>&1
