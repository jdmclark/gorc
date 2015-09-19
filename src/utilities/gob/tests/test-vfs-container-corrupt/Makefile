include ../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output

generate-output:
	-$(GOB) --jk --resource res --list >>$(RAW_OUTPUT) 2>&1
