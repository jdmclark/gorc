#include "fourcc.h"
#include "base/exception.h"

gorc::utility::fourcc::fourcc(const char* in_c) {
    int i = 0;
    for(; in_c[i]; ++i) {
        if(i >= 4) {
            throw gorc::exception();
        }

        code[i] = in_c[i];
    }

    for(; i < 4; ++i) {
        code[i] = '\0';
    }
}

gorc::utility::fourcc::fourcc(gorc::io::deserialization_constructor_tag, gorc::io::binary_input_stream& f) {
    for(int i = 0; i < 4; ++i) {
        code[i] = io::deserialize<char>(f);
    }
}

void gorc::utility::fourcc::deserialize(gorc::io::binary_input_stream& f) {
    for(int i = 0; i < 4; ++i) {
        code[i] = io::deserialize<char>(f);
    }
}

void gorc::utility::fourcc::serialize(gorc::io::binary_output_stream& f) const {
    for(int i = 0; i < 4; ++i) {
        io::serialize<char>(f, code[i]);
    }
}
