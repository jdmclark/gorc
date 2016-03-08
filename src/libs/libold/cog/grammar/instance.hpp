#pragma once

#include <string>
#include "libold/cog/ast/factory.hpp"
#include "io/input_stream.hpp"

namespace gorc {
namespace cog {
namespace grammar {

class instance {
private:
    input_stream &inputFilestream;

    // Scanner initialization members
    // implemented in gra_lexer.lex
    void init_scanner();
    void destroy_scanner();

    void* scanner;
    ast::translation_unit* return_value;

public:
    ast::factory& factory;

    instance(input_stream& inputFilestream, ast::factory& ast);
    ~instance();

    inline char get_next() {
        while(!inputFilestream.at_end()) {
            char ch;
            auto amt = inputFilestream.read_some(&ch, sizeof(char));
            if(amt == sizeof(char)) {
                return ch;
            }
        }

        return '\0';
    }

    ast::translation_unit* parse();

    inline void* get_scanner() const {
        return scanner;
    }

    inline void set_return_value(ast::translation_unit* astroot) {
        return_value = astroot;
    }

    std::string token_buffer;
};

}
}
}
