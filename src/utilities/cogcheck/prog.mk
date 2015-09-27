NAME:=cogcheck

DEPENDENCIES:= \
	libs/program \
	libs/cog/compiler \

SOURCES:= \
	cogcheck_compiler.cpp \
	disassembler.cpp \
	main.cpp \
	print_ast.cpp \
	print_ast_visitor.cpp \
