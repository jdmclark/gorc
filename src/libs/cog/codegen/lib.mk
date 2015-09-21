NAME:=codegen

DEPENDENCIES:= \
	libs/cog/ir \

SOURCES:= \
	codegen.cpp \
	lval_expression_gen_visitor.cpp \
	nonval_expression_gen_visitor.cpp \
	rval_expression_gen_visitor.cpp \
	statement_gen_visitor.cpp \
