#include "test/test.hpp"
#include "text/json_specification.hpp"
#include "io/memory_file.hpp"

using namespace gorc;

struct json_spec_test_triple {
    int a = 1;
    int b = 2;
    int c = 3;
};

begin_suite(json_specification_test);

test_case(simple)
{
    std::string doc =
        "{\n"
        "    a : 5,\n"
        "    b : 10,\n"
        "    c : 15\n"
        "}";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    json_specification<json_spec_test_triple> spec(
            /* Members */
            {
                { "a", make_json_member(&json_spec_test_triple::a) },
                { "b", make_json_member(&json_spec_test_triple::b) },
                { "c", make_json_member(&json_spec_test_triple::c) }
            }
        );

    json_spec_test_triple triple;
    gorc::json_deserialize_with_specification(jis, spec, triple);

    assert_eq(triple.a, 5);
    assert_eq(triple.b, 10);
    assert_eq(triple.c, 15);
}

test_case(redefinition)
{
    std::string doc =
        "{\n"
        "    a : 5,\n"
        "    b : 10,\n"
        "    a : 15\n"
        "}";

    gorc::memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = gorc::json_input_stream(f);

    gorc::json_specification<json_spec_test_triple> spec(
            /* Members */
            {
                { "a", make_json_member(&json_spec_test_triple::a) },
                { "b", make_json_member(&json_spec_test_triple::b) },
                { "c", make_json_member(&json_spec_test_triple::c) }
            }
        );

    json_spec_test_triple triple;
    assert_throws_logged(gorc::json_deserialize_with_specification(jis, spec, triple));

    assert_log_message(gorc::log_level::error, "<BUFFER>:4:9-4:10: element 'a' redefinition");
    assert_log_empty();
}

test_case(default_not_required)
{
    std::string doc =
        "{\n"
        "}";

    gorc::memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = gorc::json_input_stream(f);

    gorc::json_specification<json_spec_test_triple> spec(
            /* Members */
            {
                { "a", make_json_member(&json_spec_test_triple::a) },
                { "b", make_json_member(&json_spec_test_triple::b) },
                { "c", make_json_member(&json_spec_test_triple::c) }
            }
        );

    json_spec_test_triple triple;
    gorc::json_deserialize_with_specification(jis, spec, triple);

    assert_eq(triple.a, 1);
    assert_eq(triple.b, 2);
    assert_eq(triple.c, 3);
}

test_case(missing_required)
{
    std::string doc =
        "{\n"
        "    b : 10,\n"
        "}";

    gorc::memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = gorc::json_input_stream(f);

    gorc::json_specification<json_spec_test_triple> spec(
            /* Members */
            {
                { "a", make_json_member(&json_spec_test_triple::a) },
                { "b", make_json_member(&json_spec_test_triple::b) },
                { "c", make_json_member(&json_spec_test_triple::c) }
            },

            /* Required */
            { "a", "b", "c" }
        );

    json_spec_test_triple triple;
    assert_throws_logged(gorc::json_deserialize_with_specification(jis, spec, triple));

    assert_log_message(gorc::log_level::error, "<BUFFER>:3:2-3:2: a and c must be specified");
    assert_log_empty();
}

test_case(unknown_element)
{
    std::string doc =
        "{\n"
        "    a : 5,\n"
        "    b : 10,\n"
        "    c : 15,\n"
        "    d : 20,\n"
        "}";

    gorc::memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = gorc::json_input_stream(f);

    gorc::json_specification<json_spec_test_triple> spec(
            /* Members */
            {
                { "a", make_json_member(&json_spec_test_triple::a) },
                { "b", make_json_member(&json_spec_test_triple::b) },
                { "c", make_json_member(&json_spec_test_triple::c) }
            }
        );

    json_spec_test_triple triple;
    assert_throws_logged(gorc::json_deserialize_with_specification(jis, spec, triple));

    assert_log_message(gorc::log_level::error, "<BUFFER>:5:9-5:10: unknown element 'd'");
    assert_log_empty();
}

end_suite(json_specification_test);
