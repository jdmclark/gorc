NAME:=boc-shell
BIN_DIR:=$(BUILD_BIN)

DEPENDENCIES:= \
	libs/system \
	libs/program \
	libs/text \

SOURCES:= \
	lexer.cpp \
	main.cpp \

LIBRARIES:=$(SYSTEM_LIBRARIES)
