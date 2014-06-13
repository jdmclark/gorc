# Utility makefile rules and targets

BASE_PATH = .
SRC_PATH = $(BASE_PATH)/src
OBJ_PATH = $(BASE_PATH)/obj
BIN_PATH = $(BASE_PATH)/bin
HOOK_PATH = $(BASE_PATH)/scripts
TEST_PATH = $(BASE_PATH)/test

MODULE_OBJ_PATH = $(BASE_PATH)/obj/src

INTTEST_BASE_PATH = $(BASE_PATH)/test/modules
INTTEST_SRC_PATH = $(INTTEST_BASE_PATH)/src
INTTEST_BIN_PATH = $(INTTEST_BASE_PATH)/bin
INTTEST_OBJ_PATH = $(BASE_PATH)/obj/test/modules/src

CXXFLAGS = -I$(SRC_PATH) -std=gnu++11 -Werror \
	-Wall -Wextra -Wpedantic \
	-Wdouble-promotion -Wno-switch-enum -Wswitch -Wuninitialized \
	-Wstrict-aliasing=3 -Wstrict-overflow=1 \
	-Wtrampolines -Wfloat-equal -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wconversion \
	-Wlogical-op -Wno-aggressive-loop-optimizations \
	-Wvector-operation-performance -Wvla \
	-Wdisabled-optimization \
	-Wno-unused-variable -Wno-unused-local-typedefs -Wno-format \
	$(CXXFLAGS_EXTRA)

ifneq ($(shell uname -s | grep -i -e "\(mingw\|msys\)"),)
PLATFORM=mingw
else
ifneq ($(shell uname -s | grep -i -e "linux"),)
PLATFORM=linux
else
ifneq ($(shell uname -s | grep -i -e "darwin"),)
PLATFORM=macosx
else
$(error platform not recognized)
endif
endif
endif

# Add Windows package paths
ifneq ($(filter mingw,$(PLATFORM)),)
CXXFLAGS+= -static -DGLEW_STATIC -DSFML_STATIC \
		-I$(PACKAGE_DIR)/sfml/include \
		-I$(PACKAGE_DIR)/boost/include \
		-L$(PACKAGE_DIR)/sfml/lib \
		-L$(PACKAGE_DIR)/sfml/bin \
		-L$(PACKAGE_DIR)/boost/lib
endif
