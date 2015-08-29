# Rules to build and link a static library.

ifndef BUILD_INCLUDED
$(error build.mk must be included before this file)
endif

MODULE_TYPE:=library

# Only include the library again if it has not already been included.
ifeq ($(filter $(NAME),$(LIB_MODULES)),)

$(eval LIB_MODULES+=$(NAME))
$(eval include $(CREATE_MODULE))

$(eval DIRECT_DEP_LIBS:=$(NAME))
$(eval DEP_LIBS:=$(NAME) $($(NAME)_DEP_LIBS))

$(NAME)_TARGET:=$(LIB)/lib$(NAME).a

else

DIRECT_DEP_LIBS:=$(NAME)
DEP_LIBS:=$(NAME) $($(NAME)_DEP_LIBS)

endif

# Construct library link line
ifneq ($(WHOLE_ARCHIVE),)
$(NAME)_LINK_DIRECTIVE:=-Wl,--whole-archive,-l$(NAME),--no-whole-archive
else
$(NAME)_LINK_DIRECTIVE:=-l$(NAME)
endif
