include ../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output \

generate-output:
	echo "Hello, World!" \
		| $(PARENT_PROGRAM) --run $(CHILD_PROGRAM) --kill-child \
		>> $(RAW_OUTPUT) 2>&1 ; true
