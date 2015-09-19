include ../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output

generate-output:
	echo "==== default ====" >> $(RAW_OUTPUT)
	$(GOB) --jk --resource ../sample-vfs/resource --list >> $(RAW_OUTPUT)
	echo "==== mod1 ====" >> $(RAW_OUTPUT)
	$(GOB) --jk --resource ../sample-vfs/resource --game ../sample-vfs/mod1 --list >> $(RAW_OUTPUT)
	echo "==== mod2 ====" >> $(RAW_OUTPUT)
	$(GOB) --jk --resource ../sample-vfs/resource --game ../sample-vfs/mod2 --list >> $(RAW_OUTPUT)
