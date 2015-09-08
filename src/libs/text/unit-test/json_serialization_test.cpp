#include "test/test.hpp"
#include "text/json_input_stream.hpp"
#include "text/json_output_stream.hpp"
#include "io/memory_file.hpp"
#include "io/native_file.hpp"
#include "utility/make_unique.hpp"

#include <string>
#include <vector>
#include <cmath>

using namespace gorc;

struct inner_json_serializable_object {
public:
    int foo, bar;

    inner_json_serializable_object(int foo, int bar)
        : foo(foo)
        , bar(bar)
    {
        return;
    }

    inner_json_serializable_object(json_deserialization_constructor,
                                   json_input_stream &f)
    {
        json_deserialize_members(f, [this](json_input_stream &f, std::string const &mem_name)
        {
            if(mem_name == "foo") {
                foo = gorc::json_deserialize<int>(f);
            }
            else if(mem_name == "bar") {
                bar = gorc::json_deserialize<int>(f);
            }
        });
    }

    void json_serialize_object(json_output_stream& f) const
    {
        json_serialize_member(f, "foo", [&]{ gorc::json_serialize(f, foo); });
        json_serialize_member(f, "bar", [&]{ gorc::json_serialize(f, bar); });
    }
};

struct json_serializable_object {
public:
    int i, j, k;
    std::vector<int> ems;
    inner_json_serializable_object iobj;

    json_serializable_object(int i, int j)
        : i(i), j(j), k(i % j), iobj(i, j)
    {
        return;
    }

    template <typename SerializeT>
    json_serializable_object(json_deserialization_constructor, SerializeT &f)
        : iobj(0, 0)
    {
        json_deserialize_members(f, [this](json_input_stream &f, std::string const &mem_name)
        {
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

    template <typename SerializeT>
    void json_serialize_object(SerializeT &f) const
    {
        json_serialize_member(f, "i", [&]{ gorc::json_serialize(f, i); });

        // Serialize last two out of order, deliberately.
        json_serialize_member(f, "k", [&]{ gorc::json_serialize(f, k); });
        json_serialize_member(f, "j", [&]{ gorc::json_serialize(f, j); });

        json_serialize_member(f, "ems", [&]{ gorc::json_serialize_array(f, ems); });
        json_serialize_member(f, "iobj", [&]{ gorc::json_serialize_object(f, iobj); });
    }
};

begin_suite(json_serialization_test);

test_case(invalid_root_serialization)
{
    memory_file mf;
    auto f = json_output_stream(mf);

    assert_throws(json_serialize(f, 6),
                  std::logic_error,
                  "json_output_stream::assert_not_root "
                  "the selected node cannot be a root element");

    assert_throws(json_serialize(f, true),
                  std::logic_error,
                  "json_output_stream::assert_not_root "
                  "the selected node cannot be a root element");

    assert_throws(json_serialize(f, false),
                  std::logic_error,
                  "json_output_stream::assert_not_root "
                  "the selected node cannot be a root element");

    assert_throws(json_serialize_null(f),
                  std::logic_error,
                  "json_output_stream::assert_not_root "
                  "the selected node cannot be a root element");

    assert_throws(json_serialize(f, "Hello, World!\n"),
                  std::logic_error,
                  "json_output_stream::assert_not_root "
                  "the selected node cannot be a root element");

    assert_throws(json_serialize_member(f, "member_name", [&f]{ json_serialize(f, 5); }),
                  std::logic_error,
                  "json_output_stream::begin_object_element "
                  "objects cannot expect values");
}

test_case(second_root)
{
    memory_file f;
    auto jos = json_output_stream(f);

    std::vector<int> v { 0, 654, -5 };
    json_serialize_array(jos, v, [&jos](int i) { json_serialize(jos, i); });

    assert_throws(json_serialize_array(jos, v, [&jos](int i) { json_serialize(jos, i); }),
                  std::logic_error,
                  "json_output_stream::begin_field "
                  "object fields cannot be a root element");
}

test_case(array_root)
{
    memory_file f;
    auto jos = json_output_stream(f);

    std::vector<int> v { 0, 8964, 84, 28, 574, -112 };
    json_serialize_array(jos, v);

    f.set_position(0);
    auto jis = json_input_stream(f);

    std::vector<int> w;
    json_deserialize_array<int>(jis, std::back_inserter(w));

    assert_eq(v, w);
}

test_case(double_array_root)
{
    memory_file f;
    auto jos = json_output_stream(f);

    std::vector<float> v { 0.0f,
                           8964.0f,
                           84e25f,
                           28e-45f,
                           0.0574,
                           -85.372f,
                           -37.555e28f,
                           -22.9e-14f };
    json_serialize_array(jos, v);

    f.set_position(0);
    auto jis = json_input_stream(f);

    std::vector<float> dv;
    json_deserialize_array<float>(jis, std::back_inserter(dv));

    assert_true(std::equal(v.begin(), v.end(), dv.begin(), [](float a, float b) {
        return std::abs(a - b) < 0.0001f;
    }));
}

test_case(object_root)
{
    memory_file f;
    auto jos = json_output_stream(f);

    inner_json_serializable_object ijso(5, -2);
    json_serialize_object(jos, ijso);

    f.set_position(0);
    auto jis = json_input_stream(f);

    inner_json_serializable_object d_ijso(json_deserialization_constructor_tag, jis);
    assert_eq(d_ijso.foo, 5);
    assert_eq(d_ijso.bar, -2);
}

test_case(escape_strings)
{
    memory_file f;
    auto jos = json_output_stream(f);

    std::vector<std::string> grab_bag {
        "regular_string",
        "Regular string with spaces",
        "st\"ring\\ requiring es\\cap\"ing",
        "new\nline",
        "ta\tb"
    };

    json_serialize_array(jos, grab_bag);

    std::string expected_doc =
            "[\n"
            "  \"regular_string\",\n"
            "  \"Regular string with spaces\",\n"
            "  \"st\\\"ring\\\\ requiring es\\\\cap\\\"ing\",\n"
            "  \"new\\nline\",\n"
            "  \"ta\\tb\"\n"
            "]";

    // Pull the contents of the memory file
    f.set_position(0);
    std::string s;
    s.resize(f.size());
    f.read(&s[0], f.size());
    assert_eq(expected_doc, s);

    f.set_position(0);
    auto jis = json_input_stream(f);

    std::vector<std::string> d_grab_bag;
    json_deserialize_array<std::string>(jis, std::back_inserter(d_grab_bag));

    assert_eq(grab_bag, d_grab_bag);
}

test_case(nested_objects)
{
    memory_file f;
    auto jos = json_output_stream(f);

    json_serializable_object jso(5, 4);
    jso.ems.push_back(2);
    jso.ems.push_back(6847);

    json_serialize_object(jos, jso);

    f.set_position(0);
    auto jis = json_input_stream(f);

    json_serializable_object d_jso(json_deserialization_constructor_tag, jis);

    assert_eq(d_jso.i, 5);
    assert_eq(d_jso.j, 4);
    assert_eq(d_jso.k, 1);

    assert_eq(d_jso.ems, (std::vector<int> { 2, 6847 }));

    assert_eq(d_jso.iobj.foo, 5);
    assert_eq(d_jso.iobj.bar, 4);
}

test_case(booleans)
{
    memory_file f;
    auto jos = json_output_stream(f);

    std::vector<bool> booleans { true, true, false, true, false };

    json_serialize_array(jos, booleans);

    f.set_position(0);
    auto jis = json_input_stream(f);

    std::vector<bool> in_booleans;
    json_deserialize_array<bool>(jis, std::back_inserter(in_booleans));

    assert_eq(booleans, in_booleans);
}

test_case(serialize_null)
{
    class nullcontainer {
    public:
        void json_serialize_object(json_output_stream &f) const
        {
            json_serialize_member(f, "some null member", [&f]() {
                json_serialize_null(f);
            });
        }
    } nc;

    memory_file f;
    auto jos = json_output_stream(f);

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
    auto jis = json_input_stream(f);

    json_deserialize_members(jis, [](json_input_stream &jis, std::string const &mem_name) {
        assert_eq(mem_name, "some null member");
        json_deserialize_null(jis);
    });
}

test_case(eof_in_string_literal)
{
    std::string doc =
            "{\n"
            "  \"member name";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    assert_throws_logged(json_deserialize_members(jis, [](json_input_stream &,
                                                          std::string const &) {
            assert_always("object contained a member");
        }));

    assert_log_message(gorc::log_level::error,
                       "<BUFFER>:2:3-2:15: unexpected eof in string literal");
    assert_log_empty();
}

test_case(eof_in_escape_seq)
{
    std::string doc =
            "{\n"
            "  \"member name\\";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    assert_throws_logged(
        json_deserialize_members(jis, [](json_input_stream &, std::string const &) {
            assert_always("object contained a member");
        }));

    assert_log_message(gorc::log_level::error,
                       "<BUFFER>:2:3-2:16: unexpected eof in string literal escape sequence");
}

test_case(allows_bare_identifier)
{
    std::string doc =
            "{\n"
            "  _some_unknown895 : 5\n"
            "}";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    int count = 0;
    json_deserialize_members(jis, [&](json_input_stream &jis, std::string const &name) {
            assert_eq(name, "_some_unknown895");
            assert_eq(json_deserialize<int>(jis), 5);
            ++count;
        });
    assert_eq(count, 1);
}

test_case(invalid_number)
{
    std::string doc =
            "[\n"
            "  +.5eG\n"
            "]";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    std::vector<float> nums;

    assert_throws_logged(json_deserialize_array<float>(jis, std::back_inserter(nums)));

    assert_log_message(gorc::log_level::error, "<BUFFER>:2:4-2:7: expected exponent");
    assert_log_empty();
}

test_case(not_integer)
{
    std::string doc =
            "[\n"
            "  true\n"
            "]";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    std::vector<int> nums;

    assert_throws_logged(json_deserialize_array<int>(jis, std::back_inserter(nums)));

    assert_log_message(gorc::log_level::error, "<BUFFER>:2:3-2:7: expected integer");
    assert_log_empty();

    assert_eq(nums.size(), 0UL);
}

test_case(not_double)
{
    std::string doc =
            "[\n"
            "  true\n"
            "]";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    std::vector<double> nums;

    assert_throws_logged(json_deserialize_array<double>(jis, std::back_inserter(nums)));

    assert_log_message(gorc::log_level::error, "<BUFFER>:2:3-2:7: expected real");
    assert_log_empty();

    assert_eq(nums.size(), 0UL);
}

test_case(strip_trailing_zeroes)
{
    double d = 1.00000000000000001f;

    char buffer[128];
    size_t out_sz = snprintf(buffer, 128, "%#.16g", d);

    std::string strbuf(buffer);

    assert_eq(out_sz, 17UL);
    assert_eq(strbuf, "1.000000000000000");

    memory_file f;
    auto jos = json_output_stream(f);

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

test_case(invalid_escape_sequence)
{
    std::string doc =
            "[\n"
            "  \"invalid escape: \\z\"\n"
            "]";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    std::vector<std::string> strs;

    assert_throws_logged(json_deserialize_array<std::string>(jis, std::back_inserter(strs)));

    assert_log_message(gorc::log_level::error, "<BUFFER>:2:3-2:21: unknown escape sequence \\z");
    assert_log_empty();

    assert_eq(strs.size(), 0UL);
}

test_case(unknown_bareword_char)
{
    std::string doc =
            "{\n"
            "  *bareword : 5\n"
            "}";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    assert_throws_logged(
        json_deserialize_members(jis, [](json_input_stream &, std::string const &) {
            assert_always("found member");
        }));

    assert_log_message(gorc::log_level::error,
                       "<BUFFER>:2:3-2:4: expected key, found '*'");
    assert_log_empty();
}

test_case(missing_member_separator)
{
    std::string doc =
            "{\n"
            "  \"member\" 5\n"
            "}";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    assert_throws_logged(
        json_deserialize_members(jis, [](json_input_stream &, std::string const &) {
            assert_always("found member");
        }));

    assert_log_message(gorc::log_level::error,
                       "<BUFFER>:2:12-2:13: expected ':', found '5'");
    assert_log_empty();
}

test_case(null_not_bool)
{
    std::string doc =
            "[\n"
            "  null\n"
            "]";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    std::vector<bool> ems;

    assert_throws_logged(json_deserialize_array<bool>(jis, std::back_inserter(ems)));

    assert_log_message(gorc::log_level::error, "<BUFFER>:2:3-2:7: 'null' is not a boolean literal");
    assert_log_empty();
}

test_case(eof_in_array)
{
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

    auto jis = json_input_stream(f);

    std::vector<bool> ems;

    assert_throws_logged(json_deserialize_array<bool>(jis, std::back_inserter(ems)));

    assert_log_message(gorc::log_level::error,
                       "<BUFFER>:7:1-7:1: unexpected end of file inside array");
    assert_log_empty();

    assert_eq(ems.size(), 5UL);
}

test_case(eof_in_array_map)
{
    std::string doc =
            "[\n"
            "  1,\n"
            "  1,\n"
            "  1,\n"
            "  1,\n"
            "  1,\n";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    std::vector<bool> ems;

    assert_throws_logged(
        json_deserialize_array(jis, std::back_inserter(ems), [](json_input_stream &jis) {
                return json_deserialize<int>(jis);
            }));

    assert_log_message(gorc::log_level::error, "<BUFFER>:7:1-7:1: unexpected end of file inside array");
    assert_log_empty();

    assert_eq(ems.size(), 5UL);
}

test_case(array_accepts_extra_comma)
{
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

    auto jis = json_input_stream(f);

    std::vector<bool> ems;
    json_deserialize_array<bool>(jis, std::back_inserter(ems));
    assert_eq(ems.size(), 5UL);
}

test_case(eof_in_object)
{
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

    auto jis = json_input_stream(f);

    int ct = 1;

    assert_throws_logged(
        json_deserialize_members(jis, [&ct](json_input_stream &jis, std::string const &mem_name) {
            assert_eq(boost::str(boost::format("m%d") % ct), mem_name);
            assert_eq(json_deserialize<bool>(jis), true);
            ++ct;
        }));

    assert_log_message(gorc::log_level::error,
                       "<BUFFER>:7:1-7:1: unexpected end of file inside object");
    assert_log_empty();

    assert_eq(ct, 6);
}

test_case(object_accepts_extra_comma)
{
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

    auto jis = json_input_stream(f);

    int ct = 1;
    json_deserialize_members(jis, [&ct](json_input_stream &jis, std::string const &mem_name) {
        assert_eq(boost::str(boost::format("m%d") % ct), mem_name);
        assert_eq(json_deserialize<bool>(jis), true);
        ++ct;
    });

    assert_eq(ct, 6);
}

test_case(integer_formats)
{
    std::string doc = "[ 5948, 0220, 0xFEED ]";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    std::vector<int> values;
    json_deserialize_array<int>(jis, std::back_inserter(values));

    std::vector<int> expected { 5948, 144, 65261 };
    for(size_t i = 0; i < 3; ++i) {
        assert_eq(expected[i], values[i]);
    }
}

test_case(integer_signs)
{
    std::string doc = "[ 1, -1, +1 ]";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    std::vector<int> values;
    json_deserialize_array<int>(jis, std::back_inserter(values));

    std::vector<int> expected { 1, -1, 1 };
    assert_eq(expected, values);
}

test_case(serialize_object_member_without_label)
{
    memory_file f;
    auto jos = json_output_stream(f);

    class bad_json_object {
    public:
        void json_serialize_object(json_output_stream &js) const {
            json_serialize_null(js);
        }
    } bjso;

    try {
        json_serialize_object(jos, bjso);
    }
    catch(std::logic_error const &e) {
        // Success
        assert_eq(std::string(e.what()),
                  "json_output_stream::assert_element_label_printed "
                  "field names must be printed before field content");
        return;
    }

    assert_always("did not throw exception");
}

test_case(deserialize_object_from_file)
{
    auto f = make_native_read_only_file("test_files/file_test.json");
    auto jis = json_input_stream(*f);

    auto obj = json_deserialize<json_serializable_object>(jis);

    assert_eq(obj.i, 5);
    assert_eq(obj.j, 10);
    assert_eq(obj.k, 15);
}

test_case(deserialize_object_from_erroneous_file)
{
    auto f = make_native_read_only_file("test_files/file_error_test.json");
    auto jis = json_input_stream(*f);

    assert_throws_logged(json_deserialize<json_serializable_object>(jis));

    assert_log_message(gorc::log_level::error,
                       "<BUFFER>:3:9-3:13: digit out of range");
    assert_log_empty();
}

test_case(heterogeneous_objects)
{

    class hg_base {
    public:
        int em = 0;

        hg_base(json_deserialization_constructor, json_input_stream &jis)
        {
            json_deserialize_members(jis, [&](json_input_stream &jis, std::string const &member)
                {
                    if(member == "em") {
                        em = json_deserialize<int>(jis);
                    }
                });
        }

        virtual ~hg_base()
        {
            return;
        }

        virtual int em_type() = 0;
    };

    class hg_foo : public hg_base {
    public:
        using hg_base::hg_base;

        virtual int em_type() override
        {
            return 20;
        }
    };

    class hg_bar : public hg_base {
    public:
        using hg_base::hg_base;

        virtual int em_type() override
        {
            return 30;
        }
    };

    class hg_baz : public hg_base {
    public:
        using hg_base::hg_base;

        virtual int em_type() override
        {
            return 40;
        }
    };

    std::string doc =
        "[\n"
        "    foo { em : 5 },\n"
        "    bar { em : 10 },\n"
        "    baz { em : 3 },\n"
        "]";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    std::vector<std::unique_ptr<hg_base>> values;
    json_deserialize_heterogeneous_array(jis,
                                         std::back_inserter(values),
                                         [](json_input_stream &jis, std::string const &type)
                                            -> std::unique_ptr<hg_base> {
            if(type == "foo") {
                return make_unique<hg_foo>(json_deserialization_constructor_tag, jis);
            }
            else if(type == "bar") {
                return make_unique<hg_bar>(json_deserialization_constructor_tag, jis);
            }
            else {
                return make_unique<hg_baz>(json_deserialization_constructor_tag, jis);
            }
        });

    assert_eq(values.size(), size_t(3));

    assert_eq(values[0]->em, 5);
    assert_eq(values[0]->em_type(), 20);

    assert_eq(values[1]->em, 10);
    assert_eq(values[1]->em_type(), 30);

    assert_eq(values[2]->em, 3);
    assert_eq(values[2]->em_type(), 40);
}

test_case(heterogeneous_root)
{

    class hg_base {
    public:
        int em = 0;

        hg_base(json_deserialization_constructor, json_input_stream &jis)
        {
            json_deserialize_members(jis, [&](json_input_stream &jis, std::string const &member) {
                    if(member == "em") {
                        em = json_deserialize<int>(jis);
                    }
                });
        }

        virtual ~hg_base()
        {
            return;
        }
    };

    std::string doc = "foo { em: 5 }";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    std::unique_ptr<hg_base> val;
    json_deserialize_heterogeneous(jis,
                                   [&](json_input_stream &jis, std::string const &type) {
            assert_eq(type, "foo");
            val = make_unique<hg_base>(json_deserialization_constructor_tag, jis);
        });

    assert_true(val);
    assert_eq(val->em, 5);
}

test_case(invalid_type)
{
    std::string doc =
        "[\n"
        "    857 { em : 6 }\n"
        "]\n";

    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    std::vector<int> values;
    assert_throws_logged(json_deserialize_heterogeneous_array(jis,
                                                              std::back_inserter(values),
                                                              [](json_input_stream &jis,
                                                                 std::string const &) {
            int em_value = 0;
            json_deserialize_members(jis, [&](json_input_stream &jis,
                                              std::string const &) {
                    em_value = json_deserialize<int>(jis);
                });
            return em_value;
        }));
    assert_log_message(gorc::log_level::error, "<BUFFER>:2:5-2:8: expected type, found '857'");
    assert_log_empty();
}

test_case(transform_array_no_iterator)
{
    std::string doc = "[ 5, 3, 67, 2, 1 ]";
    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    std::vector<int> values;
    json_deserialize_array(jis, [&](json_input_stream &jis) {
            values.push_back(json_deserialize<int>(jis));
        });

    assert_eq(values, (std::vector<int> { 5, 3, 67, 2, 1 }));
}

test_case(unexpected_eof_in_no_iterator_array)
{
    std::string doc = "[ 5, 3, 67, 2, 1";
    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    std::vector<int> values;
    assert_throws_logged(json_deserialize_array(jis, [&](json_input_stream &jis) {
                values.push_back(json_deserialize<int>(jis));
            }));
    assert_log_message(gorc::log_level::error,
                       "<BUFFER>:1:17-1:17: unexpected end of file inside array");
    assert_log_empty();
}

test_case(deserialize_fixed_range)
{
    std::string doc = "[ 5, 4, 3, 2 ]";
    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    int values[4];
    json_deserialize_array<int>(jis, &values[0], &values[0] + 4);

    assert_eq(values[0], 5);
    assert_eq(values[1], 4);
    assert_eq(values[2], 3);
    assert_eq(values[3], 2);
}

test_case(deserialize_fixed_range_too_few)
{
    std::string doc = "[ 5, 4, 3 ]";
    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    int values[4];
    assert_throws_logged(json_deserialize_array<int>(jis, &values[0], &values[0] + 4));

    assert_log_message(gorc::log_level::error, "<BUFFER>:1:12-1:12: array contains too few elements");
    assert_log_empty();
}

test_case(deserialize_fixed_range_too_many)
{
    std::string doc = "[ 5, 4, 3, 2, 1]";
    memory_file f;
    f.write(doc.data(), doc.size());

    f.set_position(0);

    auto jis = json_input_stream(f);

    int values[4];
    assert_throws_logged(json_deserialize_array<int>(jis, &values[0], &values[0] + 4));

    assert_log_message(gorc::log_level::error, "<BUFFER>:1:15-1:16: array contains too many elements");
    assert_log_empty();
}

end_suite(json_serialization_test);
