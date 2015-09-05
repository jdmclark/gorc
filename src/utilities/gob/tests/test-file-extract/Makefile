include ../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output

generate-output:
	$(GOB) --file ../sample-vfs/resource/res.gob --extract overridden.txt >> $(RAW_OUTPUT)
	echo >> $(RAW_OUTPUT)
