#pragma once

#include <cstdio>

namespace gorc {
namespace io {

class input_stream {
public:
    virtual ~input_stream();

    virtual void read(void* dest, size_t size) = 0;
    virtual bool at_end() = 0;
};

class std_input_stream : public input_stream {
public:
    virtual void read(void* dest, size_t size) override;
    virtual bool at_end() override;
};

template <typename T> class file_input_stream : public input_stream {
private:
    T& f;

public:
    file_input_stream(T& f) : f(f) {
        return;
    }

    virtual void read(void* dest, size_t size) override {
        f.read(dest, size);
    }

    virtual bool at_end() override {
        return f.at_end();
    }
};

template <typename T> file_input_stream<T> make_input_stream(T& f) {
    return file_input_stream<T>(f);
}

}
}
