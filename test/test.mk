SRC_PATH = $(BASE_PATH)/src
OBJ_PATH = $(BASE_PATH)/obj
BIN_PATH = $(BASE_PATH)/bin
ITEST_BIN_PATH = $(BASE_PATH)/test/modules/bin

TESTSUITE_DIR = tempdir
RAW_OUTPUT = $(TESTSUITE_DIR)/raw_output.txt
CURRENT_OUTPUT = $(TESTSUITE_DIR)/current_output.txt
WORKING_OUTPUT = $(TESTSUITE_DIR)/temp_output.txt
EXPECTED_OUTPUT = expected_output.txt

.DEFAULT: all
.PHONY: all
all: create-dirs testsuite

create-dirs:
	-@rm -rf $(TESTSUITE_DIR)
	-@mkdir -p $(TESTSUITE_DIR)

process-raw-output:
	# Strip PWD from output
	-@sed 's?$(BASE_PATH)?$$ROOT$$?' $(RAW_OUTPUT) > $(CURRENT_OUTPUT)
	
compare-output:
	diff -u $(EXPECTED_OUTPUT) $(CURRENT_OUTPUT)