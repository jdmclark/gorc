#include "test/test.h"
#include "base/utility/fourcc.h"
#include "base/io/memory_file.h"
#include "base/io/binary_input_stream.h"
#include "base/io/binary_output_stream.h"
#include "base/exception.h"

begin_suite(fourcc_test);

test_case(fourcc_construction) {
    gorc::fourcc f_empty("");
    gorc::fourcc f_one("A");
    gorc::fourcc f_two("AA");
    gorc::fourcc f_three("AAA");
    gorc::fourcc f_four("AAAA");

    try {
        gorc::fourcc f_five("AAAAA");
        assert_always("did not throw exception");
    }
    catch(const gorc::exception&) { }
}

test_case(fcc_comparison) {
    gorc::fourcc fa("FEFO");
    gorc::fourcc fb("FEF0");
    gorc::fourcc fc("FEFO");

    assert_eq(fa, fa);
    assert_eq(fa, fc);
    assert_eq(fc, fa);
    assert_true(fa == fc);
    assert_true(fc == fa);
    assert_true(fa == fa);

    assert_ne(fa, fb);
    assert_ne(fb, fa);
    assert_true(fa != fb);
    assert_true(fb != fa);
}

test_case(fcc_serialization) {
    gorc::fourcc fa("FEFO");

    gorc::io::memory_file mf;
    auto bos = gorc::io::make_binary_output_stream(mf);

    gorc::io::serialize(bos, fa);

    mf.set_position(0);

    auto bis = gorc::io::make_binary_input_stream(mf);
    gorc::fourcc fb(gorc::io::deserialization_constructor, bis);

    assert_eq(fa, fb);

    mf.set_position(0);
    gorc::fourcc fc("AAAA");
    fc.deserialize(bis);

    assert_eq(fa, fc);
}

end_suite(fourcc_test);
