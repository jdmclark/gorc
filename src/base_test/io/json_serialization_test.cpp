#include "test/test.h"
#include "base/io/json_input_stream.h"
#include "base/io/json_output_stream.h"
#include "base/io/memory_file.h"
#include "base/io/exception.h"

#include <string>
#include <vector>
#include <cmath>

using namespace gorc::io;

struct inner_json_serializable_object {
public:
    int foo, bar;

    inner_json_serializable_object(int foo, int bar) : foo(foo), bar(bar) {
        return;
    }

    inner_json_serializable_object(json_constructor_tag, json_input_stream& f) {
        json_deserialize(f);
    }

    void json_deserialize(json_input_stream& f) {
        json_deserialize_members(f, [this, &f](const std::string& mem_name) {
            if(mem_name == "foo") {
                foo = gorc::io::json_deserialize<int>(f);
            }
            else if(mem_name == "bar") {
                bar = gorc::io::json_deserialize<int>(f);
            }
        });
    }

    void json_serialize(json_output_stream& f) const {
        json_serialize_member(f, "foo", [&]{ gorc::io::json_serialize(f, foo); });
        json_serialize_member(f, "bar", [&]{ gorc::io::json_serialize(f, bar); });
    }
};

struct json_serializable_object {
public:
    int i, j, k;
    std::vector<int> ems;
    inner_json_serializable_object iobj;

    json_serializable_object(int i, int j)
        : i(i), j(j), k(i % j), iobj(i, j) {
        return;
    }

    template <typename SerializeT> json_serializable_object(json_constructor_tag, SerializeT& f) : iobj(0, 0) {
        json_deserialize_members(f, [this, &f](const std::string& mem_name) {
            if(mem_name == "i") {
                i = json_deserialize<int>(f);
            }
            else if(mem_name == "j") {
                j = json_deserialize<int>(f);
            }
            else if(mem_name == "k") {
                k = json_deserialize<int>(f);
            }
            else if(mem_name == "ems") {
                json_deserialize_array<int>(f, std::back_inserter(ems));
            }
            else if(mem_name == "iobj") {
                iobj = json_deserialize<inner_json_serializable_object>(f);
            }
        });
    }

    template <typename SerializeT> void json_serialize(SerializeT& f) const {
        json_serialize_member(f, "i", [&]{ gorc::io::json_serialize(f, i); });

        // Serialize last two out of order, deliberately.
        json_serialize_member(f, "k", [&]{ gorc::io::json_serialize(f, k); });
        json_serialize_member(f, "j", [&]{ gorc::io::json_serialize(f, j); });

        json_serialize_member(f, "ems", [&]{ gorc::io::json_serialize_array(f, ems); });
        json_serialize_member(f, "iobj", [&]{ gorc::io::json_serialize_object(f, iobj); });
    }
};

begin_suite(json_serialization_test);

test_case(invalid_root_serialization) {
    memory_file mf;
    auto mfo = make_output_stream(mf);
    auto f = json_output_stream(mfo);

    try {
        json_serialize(f, 6);
        assert_always("Did not throw exception");
    }
    catch(const json_illegal_element_exception&) { }

    try {
        json_serialize(f, true);
        assert_always("Did not throw exception");
    }
    catch(const json_illegal_element_exception&) { }

    try {
        json_serialize(f, false);
        assert_always("Did not throw exception");
    }
    catch(const json_illegal_element_exception&) { }

    try {
        json_serialize_null(f);
        assert_always("Did not throw exception");
    }
    catch(const json_illegal_element_exception&) { }

    try {
        json_serialize(f, "Hello, World!\n");
        assert_always("Did not throw exception");
    }
    catch(const json_illegal_element_exception&) { }

    try {
        json_serialize_member(f, "member_name", [&f]{ json_serialize(f, 5); });
        assert_always("Did not throw exception");
    }
    catch(const json_illegal_element_exception&) { }
}

test_case(second_root) {
    memory_file f;
    auto mfo = make_output_stream(f);
    auto jos = json_output_stream(mfo);

    std::vector<int> v { 0, 654, -5 };
    json_serialize_array(jos, v, [&jos](int i) { json_serialize(jos, i); });

    try {
        json_serialize_array(jos, v, [&jos](int i) { json_serialize(jos, i); });
        assert_always("Did not throw exception");
    }
    catch(const json_illegal_element_exception&) { }
}

test_case(array_root) {
    memory_file f;
    auto mfo = make_output_stream(f);
    auto jos = json_output_stream(mfo);

    std::vector<int> v { 0, 8964, 84, 28, 574, -112 };
    json_serialize_array(jos, v);

    f.set_position(0);
    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    std::vector<int> w;
    json_deserialize_array<int>(jis, std::back_inserter(w));

    assert_eq(v, w);
}

test_case(double_array_root) {
    memory_file f;
    auto mfo = make_output_stream(f);
    auto jos = json_output_stream(mfo);

    std::vector<float> v { 0.0f, 8964.0f, 84e25f, 28e-45f, 0.0574, -85.372f, -37.555e28f, -22.9e-14f };
    json_serialize_array(jos, v);

    f.set_position(0);
    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    std::vector<float> dv;
    json_deserialize_array<float>(jis, std::back_inserter(dv));

    assert_true(std::equal(v.begin(), v.end(), dv.begin(), [](float a, float b) { return std::abs(a - b) < 0.0001f; }));
}

test_case(object_root) {
    memory_file f;
    auto mfo = make_output_stream(f);
    auto jos = json_output_stream(mfo);

    inner_json_serializable_object ijso(5, -2);
    json_serialize_object(jos, ijso);

    f.set_position(0);
    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    inner_json_serializable_object d_ijso(json_constructor, jis);
    assert_eq(d_ijso.foo, 5);
    assert_eq(d_ijso.bar, -2);
}

test_case(escape_strings) {
    memory_file f;
    auto mf = make_output_stream(f);
    auto jos = json_output_stream(mf);

    std::vector<std::string> grab_bag {
        "regular_string",
        "Regular string with spaces",
        "st\"ring\\ requiring es\\cap\"ing",
        "bell\bescaping",
        "form\ffeed",
        "new\nline",
        "carriage\rreturn",
        "ta\tb"
    };

    json_serialize_array(jos, grab_bag);

    std::string expected_doc =
            "[\n"
            "  \"regular_string\",\n"
            "  \"Regular string with spaces\",\n"
            "  \"st\\\"ring\\\\ requiring es\\\\cap\\\"ing\",\n"
            "  \"bell\\bescaping\",\n"
            "  \"form\\ffeed\",\n"
            "  \"new\\nline\",\n"
            "  \"carriage\\rreturn\",\n"
            "  \"ta\\tb\"\n"
            "]";

    // Pull the contents of the memory file
    f.set_position(0);
    std::string s;
    s.resize(f.size());
    f.read(&s[0], f.size());
    assert_eq(expected_doc, s);

    f.set_position(0);
    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    std::vector<std::string> d_grab_bag;
    json_deserialize_array<std::string>(jis, std::back_inserter(d_grab_bag));

    assert_eq(grab_bag, d_grab_bag);
}

test_case(deserialize_escaped_backslash) {
    std::string doc =
            "[\n"
            "  \"ta\\/b\"\n"
            "]";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    std::vector<std::string> strvec;
    json_deserialize_array<std::string>(jis, std::back_inserter(strvec));

    assert_eq(strvec, std::vector<std::string> { "ta/b" });
}

test_case(nested_objects) {
    memory_file f;
    auto mf = make_output_stream(f);
    auto jos = json_output_stream(mf);

    json_serializable_object jso(5, 4);
    jso.ems.push_back(2);
    jso.ems.push_back(6847);

    json_serialize_object(jos, jso);

    f.set_position(0);
    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    json_serializable_object d_jso(json_constructor, jis);

    assert_eq(d_jso.i, 5);
    assert_eq(d_jso.j, 4);
    assert_eq(d_jso.k, 1);

    assert_eq(d_jso.ems, (std::vector<int> { 2, 6847 }));

    assert_eq(d_jso.iobj.foo, 5);
    assert_eq(d_jso.iobj.bar, 4);
}

test_case(booleans) {
    memory_file f;
    auto mf = make_output_stream(f);
    auto jos = json_output_stream(mf);

    std::vector<bool> booleans { true, true, false, true, false };

    json_serialize_array(jos, booleans);

    f.set_position(0);
    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    std::vector<bool> in_booleans;
    json_deserialize_array<bool>(jis, std::back_inserter(in_booleans));

    assert_eq(booleans, in_booleans);
}

test_case(serialize_null) {
    class nullcontainer {
    public:
        void json_serialize(json_output_stream& f) const {
            json_serialize_member(f, "some null member", [&f]() { json_serialize_null(f); });
        }
    } nc;

    memory_file f;
    auto mf = make_output_stream(f);
    auto jos = json_output_stream(mf);

    json_serialize_object(jos, nc);

    std::string expected_doc =
            "{\n"
            "  \"some null member\" : null\n"
            "}";

    // Pull the contents of the memory file
    f.set_position(0);
    std::string s;
    s.resize(f.size());
    f.read(&s[0], f.size());
    assert_eq(expected_doc, s);

    f.set_position(0);
    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    json_deserialize_members(jis, [&jis](const std::string& mem_name) {
        assert_eq(mem_name, "some null member");
        json_deserialize_null(jis);
    });
}

test_case(eof_in_string_literal) {
    std::string doc =
            "{\n"
            "  \"member name";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    try {
        json_deserialize_members(jis, [&jis](const std::string&) {
            assert_always("object contained a member");
        });
    }
    catch(const json_invalid_format_exception&) {
        // Success
        return;
    }

    assert_always("did not throw exception");
}

test_case(eof_in_escape_seq) {
    std::string doc =
            "{\n"
            "  \"member name\\";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    try {
        json_deserialize_members(jis, [&jis](const std::string&) {
            assert_always("object contained a member");
        });
    }
    catch(const json_invalid_format_exception&) {
        // Success
        return;
    }

    assert_always("did not throw exception");
}

test_case(invalid_identifier) {
    std::string doc =
            "{\n"
            "  _some_unknown895 : 5\n"
            "}";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    try {
        json_deserialize_members(jis, [&jis](const std::string&) {
            assert_always("object contained a member");
        });
    }
    catch(const json_invalid_format_exception&) {
        // Success
        return;
    }

    assert_always("did not throw exception");
}

test_case(invalid_number) {
    std::string doc =
            "[\n"
            "  +.5e8\n"
            "]";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    std::vector<float> nums;
    try {
        json_deserialize_array<float>(jis, std::back_inserter(nums));
    }
    catch(const json_invalid_format_exception&) {
        // Success
        assert_eq(nums.size(), 0UL);
        return;
    }

    assert_always("did not throw exception");
}

test_case(not_integer) {
    std::string doc =
            "[\n"
            "  true\n"
            "]";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    std::vector<int> nums;
    try {
        json_deserialize_array<int>(jis, std::back_inserter(nums));
    }
    catch(const json_invalid_format_exception&) {
        // Success
        assert_eq(nums.size(), 0UL);
        return;
    }

    assert_always("did not throw exception");
}

test_case(not_double) {
    std::string doc =
            "[\n"
            "  true\n"
            "]";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    std::vector<double> nums;
    try {
        json_deserialize_array<double>(jis, std::back_inserter(nums));
    }
    catch(const json_invalid_format_exception&) {
        // Success
        assert_eq(nums.size(), 0UL);
        return;
    }

    assert_always("did not throw exception");
}

test_case(strip_trailing_zeroes) {
    double d = 1.00000000000000001f;

    char buffer[128];
    size_t out_sz = snprintf(buffer, 128, "%#.16g", d);

    std::string strbuf(buffer);

    assert_eq(out_sz, 17UL);
    assert_eq(strbuf, "1.000000000000000");

    memory_file f;
    auto mfo = make_output_stream(f);
    auto jos = json_output_stream(mfo);

    std::vector<double> dv { d };
    json_serialize_array(jos, dv);

    std::string expected_doc =
            "[\n"
            "  1.0\n"
            "]";

    // Pull the contents of the memory file
    f.set_position(0);
    std::string s;
    s.resize(f.size());
    f.read(&s[0], f.size());
    assert_eq(expected_doc, s);
}

test_case(invalid_escape_sequence) {
    std::string doc =
            "[\n"
            "  \"invalid escape: \\z\"\n"
            "]";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    std::vector<std::string> strs;
    try {
        json_deserialize_array<std::string>(jis, std::back_inserter(strs));
    }
    catch(const json_invalid_format_exception&) {
        // Success
        assert_eq(strs.size(), 0UL);
        return;
    }

    assert_always("did not throw exception");
}

test_case(unknown_bareword_char) {
    std::string doc =
            "{\n"
            "  *bareword : 5\n"
            "}";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    try {
        json_deserialize_members(jis, [&jis](const std::string&) {
            assert_always("found member");
        });
    }
    catch(const json_invalid_format_exception&) {
        // Success
        return;
    }

    assert_always("did not throw exception");
}

test_case(missing_member_separator) {
    std::string doc =
            "{\n"
            "  \"member\" 5\n"
            "}";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    try {
        json_deserialize_members(jis, [&jis](const std::string&) {
            assert_always("found member");
        });
    }
    catch(const json_invalid_format_exception&) {
        // Success
        return;
    }

    assert_always("did not throw exception");
}

test_case(null_not_bool) {
    std::string doc =
            "[\n"
            "  null\n"
            "]";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    std::vector<bool> ems;
    try {
        json_deserialize_array<bool>(jis, std::back_inserter(ems));
    }
    catch(const json_invalid_format_exception&) {
        // Success
        return;
    }

    assert_always("did not throw exception");
}

test_case(eof_in_array) {
    std::string doc =
            "[\n"
            "  true,\n"
            "  true,\n"
            "  true,\n"
            "  true,\n"
            "  true,\n";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    std::vector<bool> ems;
    try {
        json_deserialize_array<bool>(jis, std::back_inserter(ems));
    }
    catch(const json_invalid_format_exception&) {
        // Success
        assert_eq(ems.size(), 5UL);
        return;
    }

    assert_always("did not throw exception");
}

test_case(array_accepts_extra_comma) {
    std::string doc =
            "[\n"
            "  true,\n"
            "  true,\n"
            "  true,\n"
            "  true,\n"
            "  true,\n"
            "]";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    std::vector<bool> ems;
    json_deserialize_array<bool>(jis, std::back_inserter(ems));
    assert_eq(ems.size(), 5UL);
}

test_case(eof_in_object) {
    std::string doc =
            "{\n"
            "  \"m1\" : true,\n"
            "  \"m2\" : true,\n"
            "  \"m3\" : true,\n"
            "  \"m4\" : true,\n"
            "  \"m5\" : true,\n";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    int ct = 1;
    try {
        json_deserialize_members(jis, [&](const std::string& mem_name) {
            assert_eq(boost::str(boost::format("m%d") % ct), mem_name);
            assert_eq(json_deserialize<bool>(jis), true);
            ++ct;
        });
    }
    catch(const json_invalid_format_exception&) {
        // Success
        assert_eq(ct, 6);
        return;
    }

    assert_always("did not throw exception");
}

test_case(object_accepts_extra_comma) {
    std::string doc =
            "{\n"
            "  \"m1\" : true,\n"
            "  \"m2\" : true,\n"
            "  \"m3\" : true,\n"
            "  \"m4\" : true,\n"
            "  \"m5\" : true,\n"
            "}";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto mfi = make_input_stream(f);
    auto jis = json_input_stream(mfi);

    int ct = 1;
    json_deserialize_members(jis, [&](const std::string& mem_name) {
        assert_eq(boost::str(boost::format("m%d") % ct), mem_name);
        assert_eq(json_deserialize<bool>(jis), true);
        ++ct;
    });

    assert_eq(ct, 6);
}

test_case(serialize_object_member_without_label) {
    memory_file f;
    auto mfo = make_output_stream(f);
    auto jos = json_output_stream(mfo);

    class bad_json_object {
    public:
        void json_serialize(json_output_stream& js) const {
            json_serialize_null(js);
        }
    } bjso;

    try {
        json_serialize_object(jos, bjso);
    }
    catch(const json_illegal_element_exception&) {
        // Success
        return;
    }

    assert_always("did not throw exception");
}

end_suite(json_serialization_test);
