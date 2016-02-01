#pragma once

#include "cog/script/value.hpp"
#include "cog/script/message_table.hpp"
#include "io/file.hpp"
#include "io/binary_output_stream.hpp"
#include "label_id.hpp"
#include "utility/enum_hash.hpp"
#include "cog/vm/opcode.hpp"
#include <string>
#include <unordered_map>

namespace gorc {
    namespace cog {

        class ir_printer {
        private:
            file &program_text;
            binary_output_stream program_stream;

            message_table &exports;

            int next_label_id = 0;
            std::unordered_map<std::string, label_id> named_label_ids;
            std::unordered_map<message_type, label_id, enum_hash<message_type>> exported_label_ids;

            std::unordered_map<int, size_t> label_offsets;

            std::unordered_multimap<int, size_t> backpatch_map;

            bool ends_with_ret = false;

            void write_branch_instruction(opcode op, label_id lid);

        public:
            ir_printer(file &program_text,
                       message_table &exports);

            void finalize();

            label_id generate_label();
            label_id get_named_label(std::string const &);
            label_id get_export_label(message_type, std::string const &);

            void label(label_id);

            void push(value v);
            void dup();

            void load(size_t addr);
            void loadi(size_t addr);

            void stor(size_t addr);
            void stori(size_t addr);

            void jmp(label_id id);
            void jal(label_id id);
            void bt(label_id id);
            void bf(label_id id);
            void call(verb_id id);
            void callv(verb_id id);

            void ret();

            void neg();
            void lnot();

            void add();
            void sub();
            void mul();
            void div();
            void mod();

            void bor();
            void band();
            void bxor();

            void lor();
            void land();

            void eq();
            void ne();
            void gt();
            void ge();
            void lt();
            void le();
        };

    }
}
