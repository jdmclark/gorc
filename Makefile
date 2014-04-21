# Targets:
#   make all - Build the project and run tests.
#   make bin - Builds all project binaries.
#   make git-hooks - Installs git hooks.
#   make test - Build tests.
#   make clean - Removes temporary files.
#   make testsuite - Runs the integration test suite.

# ==================== #
# Module configuration #
# ==================== #

LIB_MODULES = test framework cog content game
TEST_MODULES = test_test framework_test cog_test content_test
BIN_MODULES = client server ungob

framework_LIBRARIES = GLEW GL GLU sfml-audio sfml-graphics sfml-window sfml-system boost_system boost_filesystem
content_DEPENDENCIES = cog framework
game_DEPENDENCIES = cog framework content

cog_DEPENDENCIES = framework
cog_SOURCE_FILES += $(SRC_PATH)/cog/grammar/lexer.cpp $(SRC_PATH)/cog/grammar/parser.cpp
cog_HEADER_FILES += $(SRC_PATH)/cog/grammar/parser.hpp

framework_test_DEPENDENCIES = framework
cog_test_DEPENDENCIES = cog framework content
content_test_DEPENDENCIES = cog framework content

client_DEPENDENCIES = framework cog content game
server_DEPENDENCIES = framework cog content game

ungob_DEPENDENCIES = framework cog content

INTTEST_MODULES = 

# =================== #
# Build configuration #
# ------------------- #
# Do not modify.      #
# =================== #

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

CXXFLAGS_EXTRA = -O3
CXXFLAGS = -I$(SRC_PATH) -std=gnu++11 -Werror \
	-Wall -Wextra -Wpedantic \
	-Wdouble-promotion -Wno-switch-enum -Wswitch -Wuninitialized \
	-Wstrict-aliasing=3 -Wstrict-overflow=5 \
	-Wtrampolines -Wfloat-equal -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wconversion \
	-Wlogical-op -Wno-aggressive-loop-optimizations \
	-Wvector-operation-performance -Wvla \
	-Wdisabled-optimization \
	-Wno-unused-variable \
	$(CXXFLAGS_EXTRA)

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
$(1)_INTTEST_ACTUAL_LIBRARIES += $$($(1)_INTTEST_LIBRARIES) $$(foreach depend,$$($(1)_DEPENDENCIES),$$($$(depend)_LIBRARIES))

$$(INTTEST_BIN_PATH)/$(1): $$(foreach depend,$$($(1)_INTTEST_DEPENDENCIES),$$($$(depend)_OBJECT_FILES)) $$($(1)_INTTEST_OBJECT_FILES)
	mkdir -p $$(dir $$@)
	$$(CXX) $$(CXXFLAGS) $$^ -o $$@ $$(foreach lib,$$($(1)_INTTEST_ACTUAL_LIBRARIES),-l$$(lib)) $$($(1)_INTTEST_LDFLAGS)
endef

.DEFAULT: all
.PHONY: all
all: git-hooks bin test inttest-bin
	
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

$(SRC_PATH)/cog/grammar/lexer.cpp: $(SRC_PATH)/cog/grammar/lexer.lex $(SRC_PATH)/cog/grammar/parser.hpp
	flex --outfile=$(SRC_PATH)/cog/grammar/lexer.cpp $(SRC_PATH)/cog/grammar/lexer.lex

$(SRC_PATH)/cog/grammar/lexer.cpp_EXTRA_CXXFLAGS += \
	-Wno-unused-parameter -Wno-unused-function \
	-Wno-zero-as-null-pointer-constant -Wno-conversion
    
$(SRC_PATH)/cog/grammar/parser.cpp $(SRC_PATH)/cog/grammar/parser.hpp: $(SRC_PATH)/cog/grammar/parser.y
	bison --output=$(SRC_PATH)/cog/grammar/parser.cpp $(SRC_PATH)/cog/grammar/parser.y
	
$(SRC_PATH)/cog/grammar/parser.cpp_EXTRA_CXXFLAGS += \
	-Wno-conversion -Wno-strict-overflow

$(OBJ_PATH)/%.o: $(BASE_PATH)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $($(BASE_PATH)/$(<)_EXTRA_CXXFLAGS) -MD -c $< -o $@
	@cp $(OBJ_PATH)/$*.d $(OBJ_PATH)/$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ_PATH)/$*.d >> $(OBJ_PATH)/$*.P; \
		rm -f $(OBJ_PATH)/$*.d

-include $(OBJECT_FILES:.o=.P)