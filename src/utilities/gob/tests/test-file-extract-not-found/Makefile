include ../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output

generate-output:
	-$(GOB) --file ../onefile.gob --extract foo/bar.baz 2> $(RAW_OUTPUT)
