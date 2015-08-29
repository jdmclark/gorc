# Compiler options for the test-coverage build type.

CXXFLAGS+= \
	-O0 \
	-fprofile-arcs \
	-ftest-coverage \
	-fPIC \
	-fno-elide-constructors \
