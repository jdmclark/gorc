#include "test/test.hpp"
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"
#include "io/memory_file.hpp"
#include "math/almost_equal.hpp"
#include "log/log.hpp"

using namespace gorc;

begin_suite(binary_serialization_test);

test_case(fundamental_round_trip)
{
    memory_file mf;

    binary_output_stream bos(mf);

    binary_serialize<bool>(bos, true);
    binary_serialize<bool>(bos, false);

    binary_serialize<int>(bos, 5);
    binary_serialize<float>(bos, 3.5e6f);
    binary_serialize<char>(bos, 'c');

    binary_input_stream bis(mf);

    assert_eq(binary_deserialize<bool>(bis), true);
    assert_eq(binary_deserialize<bool>(bis), false);
    assert_eq(binary_deserialize<int>(bis), 5);
    assert_true(almost_equal(binary_deserialize<float>(bis), 3.5e6f));
    assert_eq(binary_deserialize<char>(bis), 'c');
}

test_case(string_round_trip)
{
    memory_file mf;

    binary_output_stream bos(mf);
    std::string message = "Hello, World!";
    binary_serialize(bos, message);

    binary_input_stream bis(mf);
    std::string read_message = binary_deserialize<std::string>(bis);

    assert_eq(message, read_message);
}

test_case(simple_vector_round_trip)
{
    memory_file mf;

    binary_output_stream bos(mf);
    std::vector<int> values { 1, 5, 3, 7, 30, 4 };
    binary_serialize_range(bos, values);

    binary_input_stream bis(mf);
    std::vector<int> new_values;
    binary_deserialize_range<int>(bis, std::back_inserter(new_values));

    assert_eq(values, new_values);
}

test_case(fmt_fn_vector_round_trip)
{
    memory_file mf;

    binary_output_stream bos(mf);
    std::vector<int> values { 1, 5, 3, 7, 30, 4 };
    binary_serialize_range(bos, values, [](auto &bos, auto value) {
            binary_serialize(bos, value * 2);
        });

    binary_input_stream bis(mf);
    std::vector<int> new_values;
    binary_deserialize_range(bis, std::back_inserter(new_values), [](auto &bis) {
            return binary_deserialize<int>(bis) + 2;
        });

    std::vector<int> expected_values { 4, 12, 8, 16, 62, 10 };
    assert_eq(expected_values, new_values);
}

test_case(simple_object)
{
    class serializable_object {
    public:
        int value;

        serializable_object(int value)
            : value(value)
        {
            return;
        }

        serializable_object(deserialization_constructor_tag, binary_input_stream &is)
            : value(binary_deserialize<int>(is))
        {
            return;
        }

        void binary_serialize_object(binary_output_stream &os) const
        {
            binary_serialize(os, value);
        }
    };

    serializable_object my_object(12);

    memory_file mf;

    binary_output_stream bos(mf);
    binary_serialize(bos, my_object);

    binary_input_stream bis(mf);
    auto other_object = binary_deserialize<serializable_object>(bis);

    assert_eq(my_object.value, other_object.value);
}

end_suite(binary_serialization_test);
