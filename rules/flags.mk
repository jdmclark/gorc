# Defines build flags and includes platform-specific flags.

ifndef FLAGS_INCLUDED
FLAGS_INCLUDED:=1

include $(dir $(lastword $(MAKEFILE_LIST)))paths.mk

export PLATFORM:=$(shell $(SCRIPTS)/get-platform-tag)
PLATFORM_DEFINE:=PLATFORM_$(shell echo $(PLATFORM) | tr a-z A-Z)

BUILD_CONFIG?=release

MAKEFLAGS+= \
	--no-print-directory \

CFLAGS_COMMON:= \
	-D$(PLATFORM_DEFINE) \
	-I$(SRC) \
	-I$(SRC)/libs \
	-pthread \
	-Werror \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wdouble-promotion \
	-Wno-switch-enum \
	-Wswitch \
	-Wuninitialized \
	-Wstrict-aliasing=3 \
	-Wstrict-overflow=1 \
	-Wtrampolines \
	-Wfloat-equal \
	-Wcast-qual \
	-Wcast-align \
	-Wwrite-strings \
	-Wconversion \
	-Wlogical-op \
	-Wno-aggressive-loop-optimizations \
	-Wvector-operation-performance \
	-Wvla \
	-Wdisabled-optimization \
	-Wno-unused-variable \
	-Wno-unused-local-typedefs \
	-Wno-format \
	-Wl,--no-as-needed \

CXXFLAGS:= \
	-std=c++11 \
	$(CFLAGS_COMMON) \

CFLAGS:= \
	-std=c99 \
	$(CFLAGS_COMMON) \

LD_POSTFLAGS:= \
	-ldl \

include $(RULES)/config-$(BUILD_CONFIG).mk
include $(RULES)/flags-$(PLATFORM).mk

endif
