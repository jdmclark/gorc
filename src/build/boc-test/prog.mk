NAME:=boc-test
BIN_DIR:=$(BUILD_BIN)

DEPENDENCIES:= \
	libs/system \
	libs/program \
	libs/log \
	build/common \

SOURCES:= \
	find_tests.cpp \
	run_tests.cpp \
	main.cpp \

LIBRARIES:=$(SYSTEM_LIBRARIES)
