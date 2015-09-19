include ../test.mk

test: \
	generate-output \
	process-raw-output \
	compare-output

files:= \
	asset.txt \
	overridden.txt \
	modded.txt

define do_file
	echo -n "$(2): " >> $(RAW_OUTPUT)
	-$(GOB) $(1) --extract $(2) >>$(RAW_OUTPUT) 2>&1
	echo >> $(RAW_OUTPUT)

endef

define do_files
	$(foreach file,$(files),$(call do_file,$(1),$(file)))

endef

generate-output:
	echo "==== default ====" >> $(RAW_OUTPUT)
	$(call do_files,--jk --resource ../sample-vfs/resource)
	echo "==== mod1 ====" >> $(RAW_OUTPUT)
	$(call do_files,--jk --resource ../sample-vfs/resource --game ../sample-vfs/mod1)
	echo "==== mod2 ====" >> $(RAW_OUTPUT)
	$(call do_files,--jk --resource ../sample-vfs/resource --game ../sample-vfs/mod2)
	echo "==== episode1 ====" >> $(RAW_OUTPUT)
	$(GOB) --jk --resource ../sample-vfs/resource --episode ../sample-vfs/episode/test1.gob \
		--extract episode.jk | head -n 1 >> $(RAW_OUTPUT)
	echo "==== episode2 ====" >> $(RAW_OUTPUT)
	$(GOB) --jk --resource ../sample-vfs/resource --episode ../sample-vfs/episode/test2.gob \
		--extract episode.jk | head -n 1 >> $(RAW_OUTPUT)
