include ../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output

generate-output:
	$(GOB) --file ../threefiles.gob --list >> $(RAW_OUTPUT)
