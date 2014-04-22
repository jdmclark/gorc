#pragma once

#include <string>
#include "cog/ast/factory.h"
#include "base/text/source.h"
#include "base/diagnostics/report.h"

namespace gorc {
namespace cog {
namespace grammar {

class instance {
private:
    text::source& inputFilestream;

    // Scanner initialization members
    // implemented in gra_lexer.lex
    void init_scanner();
    void destroy_scanner();

    void* scanner;
    ast::translation_unit* return_value;
    const char* current_filename;

public:
    ast::factory& factory;
    diagnostics::report& report;

    instance(text::source& inputFilestream, ast::factory& ast, diagnostics::report& report);
    ~instance();

    inline char get_next() {
        return inputFilestream.get_next();
    }

    ast::translation_unit* parse();

    inline void* get_scanner() const {
        return scanner;
    }

    inline void set_return_value(ast::translation_unit* astroot) {
        return_value = astroot;
    }

    inline const char* get_filename() const {
        return current_filename;
    }

    std::string token_buffer;
};

}
}
}
