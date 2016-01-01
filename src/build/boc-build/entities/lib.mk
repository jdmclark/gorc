NAME:=boc-build-entities

DEPENDENCIES:= \
	libs/utility \
	build/boc-build/engine \
	libs/system \

SOURCES:= \
	compiler_properties.cpp \
	file_entity.cpp \
	generated_file_entity.cpp \
	gnu_compiler_properties.cpp \
	graph_entity.cpp \
	library_file_entity.cpp \
	object_file_entity.cpp \
	program_file_entity.cpp \
	project_entity.cpp \
	root_entity.cpp \
	source_file_entity.cpp \
