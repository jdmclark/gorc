#pragma once

#include "log_midend.hpp"
#include "utility/local.hpp"
#include "log_level.hpp"
#include "diagnostic_context.hpp"
#include "utility/maybe.hpp"
#include <memory>
#include <stack>

namespace gorc {

    class log_frontend : public local {
        template <typename LocalT> friend class local_factory;
        friend class diagnostic_context;
    private:
        class diagnostic_context_frame {
        public:
            bool referenced = true;
            maybe<char const *> filename;
            int first_line;
            int first_col;
            int last_line;
            int last_col;
            int internal_error_count = 0;
            size_t error_count_index;

            diagnostic_context_frame(maybe<char const *> filename,
                                     int first_line,
                                     int first_col,
                                     int last_line,
                                     int last_col,
                                     size_t error_count_index);
        };

        std::shared_ptr<log_midend> midend;
        std::vector<diagnostic_context_frame> diagnostic_context;
        bool diagnostic_preamble_dirty = false;
        std::string computed_diagnostic_preamble;

        log_frontend();

        void update_diagnostic_preamble();

        size_t push_diagnostic_context(maybe<char const *> filename,
                                       int first_line,
                                       int first_col,
                                       int last_line,
                                       int last_col);

        void release_diagnostic_context(size_t index);

    public:
        void write_log_message(std::string const &filename,
                               int line_number,
                               log_level level,
                               std::string const &message);

        int diagnostic_file_error_count() const;
        std::string diagnostic_file_name() const;
    };

    int diagnostic_file_error_count();
    std::string diagnostic_file_name();
}
