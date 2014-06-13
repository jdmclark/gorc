# Targets:
#   make all - Build the project and run tests.
#   make bin - Builds all project binaries.
#   make git-hooks - Installs git hooks.
#   make test - Build tests.
#   make clean - Removes temporary files.
#   make testsuite - Runs the integration test suite.

.DEFAULT: all
.PHONY: all
all: git-hooks bin test inttest-bin

include utility.mk
include modules.mk
include rules.mk
