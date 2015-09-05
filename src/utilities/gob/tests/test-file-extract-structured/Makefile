include ../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output

generate-output:
	$(GOB) --file ../structured.gob --extract dir/test.txt >> $(RAW_OUTPUT)
	echo >> $(RAW_OUTPUT)
