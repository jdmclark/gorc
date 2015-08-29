# Rules to build and link a single program.

ifndef BUILD_INCLUDED
$(error build.mk must be included before this file)
endif

MODULE_TYPE:=program

# Only include the program again if it has not already been included
ifeq ($(filter $(NAME),$(BIN_MODULES)),)

BIN_MODULES+=$(NAME)
$(NAME)_TARGET:=$(BIN_DIR)/$(NAME)
$(NAME)_LIBRARIES:=$(LIBRARIES)

$(eval include $(CREATE_MODULE))

endif
