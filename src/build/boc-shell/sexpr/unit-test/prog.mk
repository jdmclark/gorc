NAME:=boc-shell-sexpr-test
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/test \
	build/boc-shell/sexpr \

SOURCES:= \
	sexpr_test.cpp \
	sexpr_helpers_test.cpp \
