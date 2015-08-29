#include "log_frontend.hpp"
#include <sstream>

gorc::log_frontend::diagnostic_context_frame::diagnostic_context_frame(maybe<char const*> filename,
                                                                       int first_line,
                                                                       int first_col,
                                                                       int last_line,
                                                                       int last_col,
                                                                       size_t error_count_index)
    : filename(filename)
    , first_line(first_line)
    , first_col(first_col)
    , last_line(last_line)
    , last_col(last_col)
    , error_count_index(error_count_index)
{
    return;
}

gorc::log_frontend::log_frontend()
    : midend(get_global<log_midend>())
{
    return;
}

void gorc::log_frontend::update_diagnostic_preamble()
{
    diagnostic_preamble_dirty = false;

    if(diagnostic_context.empty()) {
        computed_diagnostic_preamble.clear();
        return;
    }

    auto const &context = diagnostic_context.back();

    std::stringstream ss;
    ss << gorc::maybe_if(context.filename, "<BUFFER>", [](char const *p) { return p; });

    if(context.first_line) {
        ss << ":" << context.first_line;
        if(context.first_col) {
            ss << ":" << context.first_col;
        }

        if(context.last_line) {
            ss << "-" << context.last_line;
            if(context.last_col) {
                ss << ":" << context.last_col;
            }
        }
    }

    ss << ": ";

    computed_diagnostic_preamble = ss.str();
}

void gorc::log_frontend::write_log_message(std::string const &filename,
                                           int line_number,
                                           log_level level,
                                           std::string const &message)
{
    if((level == log_level::error) &&
       !diagnostic_context.empty()) {
        ++diagnostic_context[diagnostic_context.back().error_count_index].internal_error_count;
    }

    if(diagnostic_preamble_dirty) {
        update_diagnostic_preamble();
    }

    midend->write_log_message(filename, line_number, level, computed_diagnostic_preamble + message);
}

size_t gorc::log_frontend::push_diagnostic_context(maybe<char const *> filename,
                                                   int first_line,
                                                   int first_col,
                                                   int last_line,
                                                   int last_col)
{
    size_t next_element = diagnostic_context.size();

    size_t error_count_index = next_element;
    if(!filename.has_value() &&
       !diagnostic_context.empty()) {
        filename = diagnostic_context.back().filename;
        error_count_index = diagnostic_context.back().error_count_index;
    }

    diagnostic_context.emplace_back(filename,
                                    first_line,
                                    first_col,
                                    last_line,
                                    last_col,
                                    error_count_index);
    diagnostic_preamble_dirty = true;

    return next_element;
}

void gorc::log_frontend::release_diagnostic_context(size_t index)
{
    if(index < diagnostic_context.size()) {
        diagnostic_context[index].referenced = false;
    }

    while(!diagnostic_context.empty() &&
          !diagnostic_context.back().referenced) {
        diagnostic_context.pop_back();
    }

    diagnostic_preamble_dirty = true;
}

int gorc::log_frontend::diagnostic_file_error_count() const
{
    if(!diagnostic_context.empty()) {
        return diagnostic_context[diagnostic_context.back().error_count_index].internal_error_count;
    }

    return 0;
}

int gorc::diagnostic_file_error_count()
{
    return get_local<log_frontend>()->diagnostic_file_error_count();
}
