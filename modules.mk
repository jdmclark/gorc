# Module configuration

LIB_MODULES = test base cog content game
TEST_MODULES = test_test base_test cog_test content_test
BIN_MODULES = client server ungob

ifneq ($(filter linux,$(PLATFORM)),)
base_LIBRARIES = GLEW GL GLU sfml-audio sfml-graphics sfml-window sfml-system \
				 boost_system boost_filesystem
else
ifneq ($(filter mingw,$(PLATFORM)),)
base_LIBRARIES = sfml-audio OpenAL32 sndfile sfml-window sfml-graphics sfml-system \
				 boost_system boost_filesystem GLEW GLU32 OpenGL32 gdi32 winmm
else
$(error platform "$(PLATFORM)"not supported)
endif
endif

content_DEPENDENCIES = cog base
game_DEPENDENCIES = cog base content

cog_DEPENDENCIES = base
cog_SOURCE_FILES += $(SRC_PATH)/cog/grammar/lexer.cpp $(SRC_PATH)/cog/grammar/parser.cpp
cog_HEADER_FILES += $(SRC_PATH)/cog/grammar/parser.hpp

$(SRC_PATH)/cog/grammar/lexer.cpp: $(SRC_PATH)/cog/grammar/lexer.lex $(SRC_PATH)/cog/grammar/parser.hpp
	flex --outfile=$(SRC_PATH)/cog/grammar/lexer.cpp $(SRC_PATH)/cog/grammar/lexer.lex

$(SRC_PATH)/cog/grammar/lexer.cpp_EXTRA_CXXFLAGS += \
	-Wno-unused-parameter -Wno-unused-function \
	-Wno-zero-as-null-pointer-constant -Wno-conversion

$(SRC_PATH)/cog/grammar/parser.cpp $(SRC_PATH)/cog/grammar/parser.hpp: $(SRC_PATH)/cog/grammar/parser.y
	bison --output=$(SRC_PATH)/cog/grammar/parser.cpp $(SRC_PATH)/cog/grammar/parser.y

$(SRC_PATH)/cog/grammar/parser.cpp_EXTRA_CXXFLAGS += \
	-Wno-conversion -Wno-strict-overflow

base_test_DEPENDENCIES = base
cog_test_DEPENDENCIES = cog base content
content_test_DEPENDENCIES = cog base content

client_DEPENDENCIES = base cog content game
server_DEPENDENCIES = base cog content game

ungob_DEPENDENCIES = base cog content

INTTEST_MODULES = input_stream native_file output_stream test_integration

test_integration_INTTEST_DEPENDENCIES += test
native_file_INTTEST_DEPENDENCIES += base
output_stream_INTTEST_DEPENDENCIES += base
input_stream_INTTEST_DEPENDENCIES += base
