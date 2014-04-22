#include "test/test.h"
#include "base/utility/entity_id.h"

using namespace gorc;

begin_suite(entity_id_test);

test_case(assignment_test) {
    entity_id id(5);
}

test_case(cast_test) {
    entity_id id(5);

    assert_eq(static_cast<int>(id), 5);
}

test_case(equality_test) {
    entity_id id(5);
    entity_id id2(5);
    entity_id id3(6);

    assert_true(id == id2);
    assert_true(id2 == id);
    assert_true(!(id == id3));
    assert_true(!(id3 == id));
    assert_eq(id, id2);
    assert_eq(id2, id);
}

test_case(inequality_test) {
    entity_id id(5);
    entity_id id2(5);
    entity_id id3(6);

    assert_true(id != id3);
    assert_true(id3 != id);
    assert_true(!(id != id2));
    assert_true(!(id2 != id));
    assert_ne(id, id3);
    assert_ne(id3, id);
}

end_suite(entity_id_test);
