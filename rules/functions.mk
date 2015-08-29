# Provides generic functions for makefiles

# Stack functions
EPSILON:=
SPACE:=$(EPSILON) $(EPSILON)

SPACE_ESCAPE:=<space>
EPSILON_ESCAPE:=<epsilon>

escape=$(if $(1),$(subst $(SPACE),$(SPACE_ESCAPE),$(1)),$(EPSILON_ESCAPE))
unescape=$(subst $(SPACE_ESCAPE),$(SPACE),$(subst $(EPSILON_ESCAPE),,$(1)))

push=$(eval $$(1):=$$(call escape,$$(2)) $$($$(1)))
pop=$(call unescape,$(firstword $($(1))))$(eval $$(1):=$$(wordlist 2,$$(words $$($$(1))),$$($$(1))))

define save_and_clear_var_uneval
$$(call push,$(value 1)_save_var_stack,$$($(value 1)))
$(value 1):=
endef

save_and_clear_var=$(eval $(call save_and_clear_var_uneval,$(1)))

save_var=$(call push,$(1)_save_var_stack,$($(1)))
restore_var=$(eval $$(1):=$$(call pop,$$(1)_save_var_stack))
