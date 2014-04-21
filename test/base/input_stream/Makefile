# Creates a simple executable which runs stdin through a std_input_stream.

include ../../test.mk

testsuite: \
	run-testsuite \
	process-raw-output \
	compare-output
	
build-testsuite:
	cp test.cpp $(TESTSUITE_DIR)/test.cpp
	$(CXX) $(CXXFLAGS_EXTRA) -std=gnu++11 -c $(TESTSUITE_DIR)/test.cpp -o $(TESTSUITE_DIR)/test.o -I$(SRC_PATH)
	$(CXX) $(CXXFLAGS_EXTRA) $(TESTSUITE_DIR)/test.o $(TOBJS) -o $(TESTSUITE_DIR)/test
	
run-testsuite:
	echo -n "Hello, World!" | $(ITEST_BIN_PATH)/input_stream > $(RAW_OUTPUT) \
		; echo "==== Testsuite returns: $$? ====" >> $(RAW_OUTPUT)
