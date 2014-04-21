#include "test/test.h"
#include "test/impl/t_strings.h"

namespace test_test {

begin_suite(strings_suite);

test_case(formattable) {
    std::string res = test::strings::conv<int>(5, "v");
    assert_eq(res, "v(5)");
}

test_case(unformattable) {
    struct unformattable_obj { } v;
    std::string res = test::strings::conv<unformattable_obj>(v, "v");
    assert_eq(res, "v");
}

end_suite(strings_suite);

}
