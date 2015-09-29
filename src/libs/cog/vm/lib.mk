NAME:=vm

DEPENDENCIES:= \
	libs/log \
	libs/io \
	libs/cog/script \

SOURCES:= \
	call_stack_frame.cpp \
	continuation.cpp \
	default_verbs.cpp \
	executor.cpp \
	heap.cpp \
	instance.cpp \
	sleep_record.cpp \
	vm.cpp \
