include ../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output

generate-output:
	echo "==== default ====" >> $(RAW_OUTPUT)
	$(GOB) --jk --resource ../sample-vfs/resource --list >> $(RAW_OUTPUT)
	echo "==== episode1 ====" >> $(RAW_OUTPUT)
	$(GOB) --jk --resource ../sample-vfs/resource --episode ../sample-vfs/episode/test1.gob --list >> $(RAW_OUTPUT)
	echo "==== episode2 ====" >> $(RAW_OUTPUT)
	$(GOB) --jk --resource ../sample-vfs/resource --episode ../sample-vfs/episode/test2.gob --list >> $(RAW_OUTPUT)
