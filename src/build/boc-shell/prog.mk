NAME:=boc-shell
BIN_DIR:=$(BUILD_BIN)

DEPENDENCIES:= \
	libs/system \
	libs/program \
	libs/text \
	libs/ast \
	libs/system \

SOURCES:= \
	ast.cpp \
	lexer.cpp \
	main.cpp \
	parser.cpp \

LIBRARIES:=$(SYSTEM_LIBRARIES)
