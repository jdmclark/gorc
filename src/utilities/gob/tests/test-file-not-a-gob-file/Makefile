include ../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output

generate-output:
	-$(GOB) --file expected-output.txt --list 2> $(RAW_OUTPUT)
