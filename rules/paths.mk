# Sets up utility path variables relative to the program root

ifndef PATHS_INCLUDED
PATHS_INCLUDED:=1

RULES:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))

export PROJECT_ROOT:=$(realpath $(RULES)/..)

SCRIPTS:=$(PROJECT_ROOT)/scripts
SRC:=$(PROJECT_ROOT)/src

PKG:=$(PROJECT_ROOT)/pkg
OBJ:=$(PKG)/obj
LIB:=$(PKG)/lib
BIN:=$(PKG)/bin
TEST_BIN:=$(PKG)/test-bin
BUILD_BIN:=$(PKG)/build-bin

FAILED_TEST_LOG:=$(PROJECT_ROOT)/test-log.txt

endif
