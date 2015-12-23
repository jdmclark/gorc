NAME:=boc-build
BIN_DIR:=$(BUILD_BIN)

DEPENDENCIES:= \
	libs/system \
	libs/program \
	libs/text \
	libs/ast \

SOURCES:= \
	main.cpp \

LIBRARIES:=$(SYSTEM_LIBRARIES)
