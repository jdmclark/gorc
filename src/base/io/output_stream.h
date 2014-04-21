#pragma once

#include <cstdio>

namespace gorc {
namespace io {

class output_stream {
public:
    virtual ~output_stream();

    virtual void write(const void* src, size_t size) = 0;
};

class std_output_stream : public output_stream {
public:
    ~std_output_stream();
    virtual void write(const void* src, size_t size) override;
};

template <typename T> class file_output_stream : public output_stream {
private:
    T& f;

public:
    file_output_stream(T& f) : f(f) {
        return;
    }

    virtual void write(const void* src, size_t size) override {
        f.write(src, size);
    }
};

template <typename T> file_output_stream<T> make_output_stream(T& f) {
    return file_output_stream<T>(f);
}

}
}
