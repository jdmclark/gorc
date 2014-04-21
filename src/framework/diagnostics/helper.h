#pragma once

#include "report.h"

namespace gorc {
namespace diagnostics {
namespace helper {

// compiler internal errors
void feature_not_implemented(report& report, const std::string& visitorname, const std::string& action);
void file_not_found(report& report, const std::string& visitorname, const std::string& filename);
void could_not_load_file(report& report, const std::string& visitorname, const std::string& filename);

// compiler lexical errors
void unrecognized_input(report& report, const std::string& input, const error_location& location);
void eof_in_string_literal(report& report, const error_location& location);
void unknown_escape_sequence(report& report, const std::string& sequence, const error_location& location);
void unescaped_newline_in_string(report& report, const error_location& location);

// compiler semantic errors
void unknown_type(report& report, const std::string& visitorname, const std::string& type, const error_location& location);
void illegal_assignment(report& report, const std::string& visitorname, const error_location& location);
void type_mismatch(report& report, const std::string& visitorname, const error_location& location);
void unknown_extension(report& report, const std::string& visitorname, const std::string& extension, const error_location& location);
void illegal_extension_assignment(report& report, const std::string& visitorname, const std::string& extension, const error_location& location);
void extension_type_mismatch(report& report, const std::string& visitorname, const std::string& extension, const error_location& location);
void symbol_redefinition(report& report, const std::string& visitorname, const std::string& name, const error_location& location);
void extension_redefinition(report& report, const std::string& visitorname, const std::string& extension, const error_location& location);
void extension_value_missing(report& report, const std::string& visitorname, const std::string& extension, const error_location& location);
void illegal_extension(report& report, const std::string& visitorname, const std::string& extension, const error_location& location);
void break_outside_loop(report& report, const std::string& visitorname, const error_location& location);
void label_inside_block(report& report, const std::string& visitorname, const std::string& labelname, const error_location& location);
void unknown_verb(report& report, const std::string& visitorname, const std::string& verbname, const error_location& location);
void invalid_argument_count(report& report, const std::string& visitorname, const std::string& verbname, size_t wanted, size_t found, const error_location& location);
void undefined_symbol(report& report, const std::string& visitorname, const std::string& symbolname, const error_location& location);
void illegal_void_result(report& report, const std::string& visitorname, const error_location& location);
void result_not_boolean(report& report, const std::string& visitorname, const error_location& location);
void call_to_unused_label(report& report, const std::string& visitorname, const std::string& labelname, const error_location& location);
void label_redefinition(report& report, const std::string& visitorname, const std::string& labelname, const error_location& location);
void missing_export(report& report, const std::string& visitorname, const std::string& labelname, const error_location& location);
void assign_to_rvalue(report& report, const std::string& visitorname, const error_location& location);
void undefined_array_base(report& report, const std::string& visitorname, const std::string& symbolname, const error_location& location);

// Other file errors
void file_corrupt(report& report, const std::string& visitorname, const error_location& location);
void expected_number(report& report, const std::string& visitorname, const error_location& location);
void expected_punctuator(report& report, const std::string& visitorname, const std::string& found, const std::string& expected, const error_location& location);
void expected_identifier(report& report, const std::string& visitorname, const std::string& found, const std::string& expected, const error_location& location);
void expected_filename(report& report, const std::string& visitorname, const error_location& location);
void expected_string(report& report, const std::string& visitorname, const error_location& location);
void expected_label(report& report, const std::string& visitorname, const std::string& found, const std::string& expected, const error_location& location);
void expected_end_of_file(report& report, const std::string& visitorname, const std::string& found, const error_location& location);
void unexpected_end_of_file_in_string(report& report, const std::string& visitorname, const error_location& location);
void unexpected_end_of_line_in_string(report& report, const std::string& visitorname, const error_location& location);
void expected(report& report, const std::string& visitorname, const std::string& expected, const error_location& location);
void unexpected_end_of_file(report& report, const std::string& visitorname, const error_location& location);
void unexpected_end_of_line(report& report, const std::string& visitorname, const error_location& location);
void template_redefinition(report& report, const std::string& visitorname, const std::string& tplname, const error_location& location);

}
}
}
