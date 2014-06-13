# Shared build rules and targets

MODULES = $(LIB_MODULES) $(TEST_MODULES) $(BIN_MODULES)

define MODULE_template
$(1)_SOURCE_FILES += $$(shell find $$(SRC_PATH)/$(1) -name *.cpp)
$(1)_HEADER_FILES += $$(shell find $$(SRC_PATH)/$(1) -name *.h)
$(1)_OBJECT_FILES += $$(subst $$(SRC_PATH),$$(MODULE_OBJ_PATH),$$($(1)_SOURCE_FILES:.cpp=.o))
SOURCE_FILES += $$($(1)_SOURCE_FILES)
HEADER_FILES += $$($(1)_HEADER_FILES)
OBJECT_FILES += $$($(1)_OBJECT_FILES)
$(1)_ACTUAL_LIBRARIES += $$($(1)_LIBRARIES) $$(foreach depend,$$($(1)_DEPENDENCIES),$$($$(depend)_LIBRARIES))
endef

define TEST_MODULE_template
$(1)_OBJECT_FILES += $(test_OBJECT_FILES)
endef

define BIN_MODULE_template
$$(BIN_PATH)/$(1): $$(foreach depend,$$($(1)_DEPENDENCIES),$$($$(depend)_OBJECT_FILES)) $$($(1)_OBJECT_FILES)
	mkdir -p $$(dir $$@)
	$$(CXX) $$(CXXFLAGS) $$^ -o $$@ $$(foreach lib,$$($(1)_ACTUAL_LIBRARIES),-l$$(lib)) $$($(1)_LDFLAGS)
endef

define INTTEST_BIN_MODULE_template
$(1)_INTTEST_SOURCE_FILES += $$(shell find $$(INTTEST_SRC_PATH)/$(1) -name *.cpp)
$(1)_INTTEST_HEADER_FILES += $$(shell find $$(INTTEST_SRC_PATH)/$(1) -name *.h)
$(1)_INTTEST_OBJECT_FILES += $$(subst $$(INTTEST_SRC_PATH),$$(INTTEST_OBJ_PATH),$$($(1)_INTTEST_SOURCE_FILES:.cpp=.o))
$(1)_INTTEST_ACTUAL_LIBRARIES += $$($(1)_INTTEST_LIBRARIES) $$(foreach depend,$$($(1)_INTTEST_DEPENDENCIES),$$($$(depend)_LIBRARIES))

$$(INTTEST_BIN_PATH)/$(1): $$(foreach depend,$$($(1)_INTTEST_DEPENDENCIES),$$($$(depend)_OBJECT_FILES)) $$($(1)_INTTEST_OBJECT_FILES)
	mkdir -p $$(dir $$@)
	$$(CXX) $$(CXXFLAGS) $$^ -o $$@ $$(foreach lib,$$($(1)_INTTEST_ACTUAL_LIBRARIES),-l$$(lib)) $$($(1)_INTTEST_LDFLAGS)
endef

test: $(foreach test,$(TEST_MODULES),$(BIN_PATH)/$(test))

bin: $(foreach bin,$(BIN_MODULES),$(BIN_PATH)/$(bin))

inttest-bin: $(foreach bin,$(INTTEST_MODULES),$(INTTEST_BIN_PATH)/$(bin))

.PHONY: clean
clean:
	-@rm -rf $(BIN_PATH) $(OBJ_PATH) $(INTTEST_BIN_PATH) 2> /dev/null || true
	-@find $(TEST_PATH) -type f -name test_log.txt -exec rm -f {} \;
	-@find $(TEST_PATH) -depth -type d -name tempdir -exec rm -rf {} \;

.PHONY: git-hooks
git-hooks:
	@cp $(HOOK_PATH)/pre-commit .git/hooks/pre-commit

testsuite: test inttest-bin
	-@echo "=============================="
	-@echo "Running unit test suite"
	-@echo "=============================="
	@ $(foreach test,$(TEST_MODULES),$(BIN_PATH)/$(test) &&) :
	@ CXXFLAGS_EXTRA="$(CXXFLAGS_EXTRA)" $(HOOK_PATH)/testsuite

$(foreach module,$(MODULES),$(eval $(call MODULE_template,$(module))))
$(foreach module,$(BIN_MODULES),$(eval $(call BIN_MODULE_template,$(module))))
$(foreach module,$(TEST_MODULES),$(eval $(call TEST_MODULE_template,$(module))))
$(foreach module,$(TEST_MODULES),$(eval $(call BIN_MODULE_template,$(module))))
$(foreach module,$(INTTEST_MODULES),$(eval $(call INTTEST_BIN_MODULE_template,$(module))))

$(OBJ_PATH)/%.o: $(BASE_PATH)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $($(BASE_PATH)/$(<)_EXTRA_CXXFLAGS) -MD -c $< -o $@
	@cp $(OBJ_PATH)/$*.d $(OBJ_PATH)/$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ_PATH)/$*.d >> $(OBJ_PATH)/$*.P; \
		rm -f $(OBJ_PATH)/$*.d

-include $(OBJECT_FILES:.o=.P)
