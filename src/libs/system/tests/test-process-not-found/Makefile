include ../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output \

generate-output:
	echo "Hello, World!" \
		| $(PARENT_PROGRAM) --run doesnotexist \
		>> $(RAW_OUTPUT)
