# Targets:
#   make all - Build the project and run tests.
#   make test - Build tests (requires NullUnit).
#   make clean - Removes temporary files.

# ==================== #
# Module configuration #
# ==================== #

LIB_MODULES = framework cog content
TEST_MODULES = frameworktest cogtest contenttest
BIN_MODULES = game

framework_LIBRARIES = boost_filesystem boost_system
content_LIBRARIES = sfml-audio sfml-graphics sfml-window sfml-system BulletDynamics BulletCollision LinearMath
content_DEPENDENCIES = cog framework

cog_DEPENDENCIES = framework
cog_SOURCE_FILES += $(SRC_PATH)/cog/grammar/lexer.cpp $(SRC_PATH)/cog/grammar/parser.cpp
cog_HEADER_FILES += $(SRC_PATH)/cog/grammar/parser.hpp

frameworktest_DEPENDENCIES = framework
cogtest_DEPENDENCIES = cog framework content
contenttest_DEPENDENCIES = cog framework content

game_DEPENDENCIES = framework cog content
game_LIBRARIES = GL GLU

# =================== #
# Build configuration #
# ------------------- #
# Do not modify.      #
# =================== #

BASE_PATH = .
SRC_PATH = $(BASE_PATH)/src
OBJ_PATH = $(BASE_PATH)/obj
BIN_PATH = $(BASE_PATH)/bin

CPPFLAGS = -I$(SRC_PATH) -I/usr/include/bullet
CXXFLAGS = -std=c++11 -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-missing-braces -ggdb

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
$(1)_ACTUAL_LIBRARIES += nullunit
endef

define BIN_MODULE_template
$$(BIN_PATH)/$(1): $$(foreach depend,$$($(1)_DEPENDENCIES),$$($$(depend)_OBJECT_FILES)) $$($(1)_OBJECT_FILES)
	mkdir -p $$(dir $$@)
	$$(CXX) $$(CPPFLAGS) $$(CXXFLAGS) $$^ -o $$@ $$(foreach lib,$$($(1)_ACTUAL_LIBRARIES),-l$$(lib))
endef

.DEFAULT: all
.PHONY: all
all: test bin
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
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MD -c $< -o $@
	@cp $(OBJ_PATH)/$*.d $(OBJ_PATH)/$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(OBJ_PATH)/$*.d >> $(OBJ_PATH)/$*.P; \
		rm -f $(OBJ_PATH)/$*.d

-include $(OBJECT_FILES:.o=.P)
