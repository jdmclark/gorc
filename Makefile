# Targets:
#   make all - Build the project and run tests.
#   make bin - Builds all project binaries.
#   make git-hooks - Installs git hooks.
#   make test - Build tests.
#   make clean - Removes temporary files.

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

CXXFLAGS = -I$(SRC_PATH) -std=gnu++11 -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -O3

MODULES = $(LIB_MODULES) $(TEST_MODULES) $(BIN_MODULES)

define MODULE_template
$(1)_SOURCE_FILES += $$(shell find $$(SRC_PATH)/$(1) -name *.cpp)
$(1)_HEADER_FILES += $$(shell find $$(SRC_PATH)/$(1) -name *.h)
$(1)_OBJECT_FILES += $$(subst $$(SRC_PATH),$$(OBJ_PATH),$$($(1)_SOURCE_FILES:.cpp=.o))
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

.DEFAULT: all
.PHONY: all
all: git-hooks test bin
	@ $(foreach test,$(TEST_MODULES),$(BIN_PATH)/$(test) &&) :
	
.PHONY: test
test: $(foreach test,$(TEST_MODULES),$(BIN_PATH)/$(test))

.PHONY: bin
bin: $(foreach bin,$(BIN_MODULES),$(BIN_PATH)/$(bin))

.PHONY: clean
clean:
	-@rm -rf $(BIN_PATH) $(OBJ_PATH) 2> /dev/null || true
	-@rm $(SRC_PATH)/cog/grammar/lexer.cpp $(SRC_PATH)/cog/grammar/parser.cpp 2> /dev/null || true
	-@rm $(SRC_PATH)/cog/grammar/parser.hpp $(SRC_PATH)/cog/grammar/parser.output 2> /dev/null || true

.PHONY: git-hooks
git-hooks:
	@cp $(HOOK_PATH)/pre-commit .git/hooks/pre-commit
    
$(foreach module,$(MODULES),$(eval $(call MODULE_template,$(module))))
$(foreach module,$(BIN_MODULES),$(eval $(call BIN_MODULE_template,$(module))))
$(foreach module,$(TEST_MODULES),$(eval $(call TEST_MODULE_template,$(module))))
$(foreach module,$(TEST_MODULES),$(eval $(call BIN_MODULE_template,$(module))))

$(SRC_PATH)/cog/grammar/lexer.cpp: $(SRC_PATH)/cog/grammar/lexer.lex $(SRC_PATH)/cog/grammar/parser.hpp
	flex --outfile=$(SRC_PATH)/cog/grammar/lexer.cpp $(SRC_PATH)/cog/grammar/lexer.lex
    
$(SRC_PATH)/cog/grammar/parser.cpp $(SRC_PATH)/cog/grammar/parser.hpp: $(SRC_PATH)/cog/grammar/parser.y
	bison --output=$(SRC_PATH)/cog/grammar/parser.cpp $(SRC_PATH)/cog/grammar/parser.y
    
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MD -c $< -o $@
	@cp $(OBJ_PATH)/$*.d $(OBJ_PATH)/$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ_PATH)/$*.d >> $(OBJ_PATH)/$*.P; \
		rm -f $(OBJ_PATH)/$*.d

-include $(OBJECT_FILES:.o=.P)