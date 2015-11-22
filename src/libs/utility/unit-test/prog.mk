NAME:=utility-test
BIN_DIR:=$(TEST_BIN)

EXCLUDE_TEST_COVERAGE:=1

DEPENDENCIES:= \
	libs/test \
	libs/utility \

SOURCES:= \
	abs_test.cpp \
	apply_test.cpp \
	contains_type_test.cpp \
	flag_set_test.cpp \
	foreach_test.cpp \
	gcd_test.cpp \
	global_test.cpp \
	lcm_test.cpp \
	local_test.cpp \
	make_unique_test.cpp \
	maybe_test.cpp \
	mod_test.cpp \
	oxford_join_test.cpp \
	range_test.cpp \
	runtime_assert_test.cpp \
	service_registry_test.cpp \
	strcat_test.cpp \
	string_search_test.cpp \
	string_view_test.cpp \
	variant_test.cpp \
	wrapped_test.cpp \
	zip_test.cpp \
