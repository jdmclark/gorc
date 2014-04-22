#include "test/test.h"
#include "base/math/matrix.h"

using namespace gorc;

begin_suite(matrix_test);

test_case(exists) {
    auto m = make_matrix<2, int>(make_vector(1, 0),
                                 make_vector(0, 1));
    assert_eq((get<0, 0>(m)), 1);
    assert_eq((get<0, 1>(m)), 0);
    assert_eq((get<1, 0>(m)), 0);
    assert_eq((get<1, 1>(m)), 1);
}

end_suite(matrix_test);
