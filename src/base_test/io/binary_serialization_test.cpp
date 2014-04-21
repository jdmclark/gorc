#include "test/test.h"
#include "base/io/binary_input_stream.h"
#include "base/io/binary_output_stream.h"
#include "base/io/memory_file.h"

#include <string>
#include <vector>

using namespace gorc::io;

struct serializable_object {
public:
    int i, j, k;

    serializable_object(int i, int j)
        : i(i), j(j), k(i % j) {
        return;
    }

    template <typename SerializeT> serializable_object(deserialization_constructor_tag, SerializeT& f) {
        deserialize(f, i);
        deserialize(f, j);
        deserialize(f, k);
    }

    template <typename SerializeT> void serialize(SerializeT& f) const {
        gorc::io::serialize(f, i);
        gorc::io::serialize(f, j);
        gorc::io::serialize(f, k);
    }
};

begin_suite(binary_serialization_test);

test_case(serialization_deserialization) {
    memory_file f;

    auto bos = make_binary_output_stream(f);

    // Serialize POD:
    serialize<int32_t>(bos, 38574);

    // Serialize string:
    std::string message("Hello, World!\n");
    serialize_range(bos, message);

    // Serialize vector of PODs:
    std::vector<int> pods { 0, 6, 84, 3, -7 };
    serialize_range(bos, pods);

    // Serialize serializable object:
    serializable_object so(5, 3);
    serialize(bos, so);

    // Serialize vector of objects:
    std::vector<serializable_object> sos;
    for(int i = 0; i < 7; ++i) {
        sos.emplace_back(i, 3);
    }

    serialize_range(bos, sos);


    f.set_position(0);

    auto bis = make_binary_input_stream(f);

    // Deserialize POD:
    int32_t pod;
    deserialize(bis, pod);
    assert_eq(pod, 38574);

    // Deserialize string:
    std::string d_message;
    deserialize_range<char>(bis, std::back_inserter(d_message));
    assert_eq(d_message, "Hello, World!\n");

    // Deserialize vector of PODs:
    std::vector<int> d_pods;
    deserialize_range<int>(bis, std::back_inserter(d_pods));
    assert_eq(d_pods, pods);

    // Deserialize deserializable object:
    serializable_object d_so(deserialization_constructor, bis);
    assert_eq(d_so.i, 5);
    assert_eq(d_so.j, 3);
    assert_eq(d_so.k, 2);

    // Deserialize range of objects:
    std::vector<serializable_object> d_sos;
    deserialize_range<serializable_object>(bis, std::back_inserter(d_sos));

    for(int i = 0; i < 7; ++i) {
        const auto& em = d_sos[i];

        assert_eq(em.i, i);
        assert_eq(em.j, 3);
        assert_eq(em.k, (i % 3));
    }

    assert_true(f.at_end());
}

end_suite(binary_serialization_test);
