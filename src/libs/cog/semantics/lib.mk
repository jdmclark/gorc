NAME:=semantics

DEPENDENCIES:= \
	libs/cog/script \
	libs/cog/ast \

SOURCES:= \
	analyzer.cpp \
	code_visitor.cpp \
	collect_labels.cpp \
	collect_symbols.cpp \
	flow_visitor.cpp \
	for_expression_visitor.cpp \
	lvalue_visitor.cpp \
	rvalue_visitor.cpp \
	symbol_extension_field_integer_visitor.cpp \
	symbol_extension_field_string_visitor.cpp \
	symbol_extension_type.cpp \
	symbol_extension_visitor.cpp \
	symbol_field_visitor.cpp \
	variant_location_visitor.cpp \
