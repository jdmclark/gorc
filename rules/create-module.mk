# Common rules to build and link a module.

ifndef BUILD_INCLUDED
$(error build.mk must be included before this file)
endif

# Assert that this module is uniquely named
ifeq ($(NAME),)
$(error $(MODULE_TYPE) must specify a name)
endif

ifneq ($(filter $(NAME),$(MODULES)),)
$(error $(MODULE_TYPE) $(NAME) is not uniquely named)
endif

$(info Adding $(MODULE_TYPE) $(NAME) to targets)

$(eval MODULES+=$(NAME))

$(NAME)_SOURCE_FILES:=$(foreach src,$(strip $(SOURCES)),$(abspath $(MODULE_SRC)/$(src)))

# Compute primary object files for this module
$(NAME)_OBJECT_FILES:=$(subst $(PROJECT_ROOT),$(OBJ),\
	$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$($(NAME)_SOURCE_FILES))))

# Add source directory to coverage exclusion
ifneq ($(EXCLUDE_TEST_COVERAGE),)
$(eval EXCLUDE_TEST_COVERAGE_PATHS+=$(MODULE_SRC))
endif

$(NAME)_DEP_LIBS:=

$(NAME)_DEPENDENCIES:=$(strip $(DEPENDENCIES))
$(NAME)_UTILITIES:=$(strip $(UTILITIES))

# Add dependency targets
$(foreach dep,$($(NAME)_DEPENDENCIES),\
	$(call save_and_clear_var,NAME) \
	$(eval MODULE_SRC:=$(realpath $(SRC)/$(dep))) \
	$(eval BIN_DIR:=$(BIN)) \
	$(foreach var,$(strip $(MODULE_VARS)),$(eval $(var):=)) \
	$(eval include $(MODULE_SRC)/lib.mk $(CREATE_LIB)) \
	$(call restore_var,NAME) \
	$(eval $(NAME)_DIRECT_DEP_LIBS+=$(filter-out $($(NAME)_DIRECT_DEP_LIBS),$(DIRECT_DEP_LIBS))) \
	$(eval $(NAME)_DEP_LIBS+=$(filter-out $($(NAME)_DEP_LIBS),$(DEP_LIBS))) \
)

# Add utility targets
$(foreach dep,$($(NAME)_UTILITIES),\
	$(call save_and_clear_var,NAME) \
	$(eval MODULE_SRC:=$(realpath $(SRC)/$(dep))) \
	$(eval BIN_DIR:=$(BIN)) \
	$(foreach var,$(strip $(MODULE_VARS)),$(eval $(var):=)) \
	$(eval include $(MODULE_SRC)/$(strip $(PROG_NAME_OVERRIDE)) $(CREATE_PROG)) \
	$(call restore_var,NAME) \
)

DEP_LIBS:=$($(NAME)_DEP_LIBS)
