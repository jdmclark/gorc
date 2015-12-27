NAME:=boc-build
BIN_DIR:=$(BUILD_BIN)

DEPENDENCIES:= \
	libs/system \
	libs/program \
	libs/text \
	libs/ast \
	libs/log \
	build/boc-build/engine \
	build/boc-build/entities \

SOURCES:= \
	main.cpp \
	paths.cpp \

LIBRARIES:=$(SYSTEM_LIBRARIES)
