include ../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output

generate-output:
	-$(GOB) --jk --resource ../sample-vfs/resource --extract foo >>$(RAW_OUTPUT) 2>&1
