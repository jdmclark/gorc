include ../test.mk

PROGRAM=$(TEST_BIN)/example-program

test: \
	generate-output \
	process-raw-output \
	compare-output

generate-output:
	echo "==== shell code ====" >> $(RAW_OUTPUT)
	$(PROGRAM) --fatal-message "Fatality"; echo $$? >> $(RAW_OUTPUT)
	echo "==== standard output ====" >> $(RAW_OUTPUT)
	$(PROGRAM) --fatal-message "Fatality" 2>&1 \
		| grep ".*" >> $(RAW_OUTPUT)
