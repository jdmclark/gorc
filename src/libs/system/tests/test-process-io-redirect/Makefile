include ../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output \

generate-output:
	echo "Hello, World!" \
		| $(PARENT_PROGRAM) --run $(CHILD_PROGRAM) \
		>> $(RAW_OUTPUT)
