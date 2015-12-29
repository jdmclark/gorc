NAME:=boc-build-engine

DEPENDENCIES:= \
	libs/utility \
	libs/io \
	libs/log \

SOURCES:= \
	adjacency_list.cpp \
	base_file_entity.cpp \
	dirty_list.cpp \
	entity.cpp \
	entity_allocator.cpp \
	entity_closure.cpp \
	entity_deserializer.cpp \
	entity_registry.cpp \
	entity_scheduler.cpp \
	entity_serializer.cpp \
