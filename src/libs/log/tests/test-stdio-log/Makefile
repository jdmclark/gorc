include ../../../../../rules/test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output

generate-output:
	$(TEST_BIN)/example-stdio-log 2> $(RAW_OUTPUT)
