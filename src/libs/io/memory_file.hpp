#pragma once

#include "file.hpp"
#include "read_only_file.hpp"
#include <vector>

namespace gorc {

    class memory_file {
    public:
        class reader : public read_only_file {
        private:
            memory_file const &mf;
            size_t offset = 0;

        public:
            reader(memory_file const &mf);

            virtual size_t read_some(void *dest, size_t size) override;

            virtual void seek(ssize_t offset) override;
            virtual void set_position(size_t offset) override;
            virtual size_t position() override;

            virtual size_t size() override;
            virtual bool at_end() override;
        };

        class writer : public file {
        private:
            memory_file &mf;
            size_t offset = 0;

        public:
            writer(memory_file &mf);

            virtual size_t write_some(void const *src, size_t size) override;
            virtual void set_position(size_t offset) override;
            virtual size_t position() override;
        };

        friend class reader;
        friend class writer;

    private:
        std::vector<char> buffer;

        reader reader_instance;
        writer writer_instance;

    public:
        memory_file();

        void write(void const *src, size_t size);
        size_t write_some(void const *src, size_t size);

        void read(void *dest, size_t size);
        size_t read_some(void *dest, size_t size);

        void seek(ssize_t offset);
        void set_position(size_t offset);
        size_t position();

        size_t size();
        bool at_end();

        void copy_to(output_stream&) const;

        inline operator reader&()
        {
            return reader_instance;
        }

        inline operator writer&()
        {
            return writer_instance;
        }
    };

}
