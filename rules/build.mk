# Specifies build system rules for binary programs.

ifndef BUILD_INCLUDED
BUILD_INCLUDED:=1

include $(dir $(lastword $(MAKEFILE_LIST)))flags.mk
include $(RULES)/functions.mk

# Add function-like makefiles
CREATE_MODULE:=$(RULES)/create-module.mk
CREATE_PROG:=$(RULES)/create-prog.mk
CREATE_LIB:=$(RULES)/create-lib.mk

# Find all prog.mk files relative to the CURRENT directory
PROG_NAME_OVERRIDE:=prog.mk
PROGS:=$(shell find . -type f -name $(strip $(PROG_NAME_OVERRIDE)))

MODULE_VARS:= \
	SOURCES \
	DEPENDENCIES \
	DIRECT_DEP_LIBS \
	DEP_LIBS \
	LIBRARIES \
	EXCLUDE_TEST_COVERAGE \
	WHOLE_ARCHIVE \

NAME:=
BIN_MODULES:=
EXCLUDE_TEST_COVERAGE_PATHS:=

$(foreach prog,$(PROGS),\
	$(eval MODULE_SRC:=$(realpath $(dir $(prog)))) \
	$(eval BIN_DIR:=$(BIN)) \
	$(foreach var,$(strip $(MODULE_VARS)),$(eval $(var):=)) \
	$(eval include $(prog) $(CREATE_PROG)) \
)

define lib_module_template
-include $$($(1)_OBJECT_FILES:.o=.P)

$$($(1)_TARGET): $$($(1)_OBJECT_FILES)
	mkdir -p $$(dir $$@)
	$$(AR) rcsT $$@ $$^

endef

$(foreach lib,$(LIB_MODULES),$(eval $(call lib_module_template,$(lib))))

define bin_module_template
-include $$($(1)_OBJECT_FILES:.o=.P)

$(1)_PAIRED_DEP_LIBS:= \
	$$(strip \
		$$(foreach lib,$$($(1)_DEP_LIBS),\
			$$(lib) $$(lib) \
			$$(foreach dep,$$($$(lib)_DIRECT_DEP_LIBS),\
				$$(lib) $$(dep))) \
		)

$(1)_SORTED_DEP_LIBS:= \
	$$(shell echo $$($(1)_PAIRED_DEP_LIBS) | tsort -)

$$($(1)_TARGET): \
 $$($(1)_OBJECT_FILES) \
 $$(foreach lib,$$($(1)_DEP_LIBS),$$($$(lib)_TARGET))
	mkdir -p $$(dir $$@)
	$$(CXX) $$(CXXFLAGS) $$($(1)_OBJECT_FILES) -o $$@ \
		-L$(LIB) $$(foreach lib,$$($(1)_SORTED_DEP_LIBS),$$($$(lib)_LINK_DIRECTIVE)) \
		$$($(1)_LIBRARIES) $$(DEFAULT_LIBRARIES) $$(LD_POSTFLAGS)

endef

$(foreach bin,$(BIN_MODULES),$(eval $(call bin_module_template,$(bin))))

# Compile bin source files
$(OBJ)/%.o: $(PROJECT_ROOT)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(CXX) $(CXXFLAGS) $< -MQ "$@" -E -MM -MP -o $(@:.o=.P)

$(OBJ)/%.o: $(PROJECT_ROOT)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@
	$(CC) $(CCFLAGS) $< -MQ "$@" -E -MM -MP -o $(@:.o=.P)

.PHONY: build-all-bin-modules
build-all-bin-modules: $(foreach bin,$(BIN_MODULES),$($(bin)_TARGET))

.PHONY: generate-test-coverage-report
generate-test-coverage-report:
	gcovr --root=$(PROJECT_ROOT) \
		--sort-uncovered \
		--exclude-unreachable-branches \
		$(foreach excl,$(EXCLUDE_TEST_COVERAGE_PATHS),--exclude $(excl))

.PHONY: generate-coveralls-test-coverage-report
generate-coveralls-test-coverage-report:
	cpp-coveralls --gcov-options '\-lp' \
		--root $(SRC) \
		$(foreach excl,$(EXCLUDE_TEST_COVERAGE_PATHS),--exclude $(excl))

endif
