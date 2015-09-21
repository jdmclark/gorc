#pragma once

#include "file.hpp"
#include "read_only_file.hpp"
#include "path.hpp"
#include <cstdio>
#include <memory>

namespace gorc {

    class native_file : public file {
    private:
        friend std::unique_ptr<native_file> make_native_file(path const &filename);

        FILE *file_handle;

    public:
        native_file(path const &filename);
        ~native_file();

        native_file(native_file const&) = delete;
        native_file& operator=(native_file const&) = delete;

        virtual size_t write_some(void const *src, size_t size) override;

        virtual void set_position(size_t offset) override;
        virtual size_t position() override;
    };

    class native_read_only_file : public read_only_file {
    private:
        friend std::unique_ptr<native_read_only_file> make_native_read_only_file(path const &filename);

        path filename;
        FILE *file_handle;

    public:
        native_read_only_file(path const &filename);
        ~native_read_only_file();

        native_read_only_file(native_read_only_file const&) = delete;
        native_read_only_file& operator=(native_read_only_file const&) = delete;

        virtual size_t read_some(void *dest, size_t size) override;

        virtual void seek(ssize_t offset) override;
        virtual void set_position(size_t offset) override;
        virtual size_t position() override;

        virtual size_t size() override;
        virtual bool at_end() override;
    };

    std::unique_ptr<native_file> make_native_file(path const &filename);
    std::unique_ptr<native_read_only_file> make_native_read_only_file(path const &filename);

}
