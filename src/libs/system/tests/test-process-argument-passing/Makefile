include ../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output \

generate-output:
	echo "Hello, World!" \
		| $(PARENT_PROGRAM) --run $(CHILD_PROGRAM) --extra --fail --extra --failmsg \
		>> $(RAW_OUTPUT)
