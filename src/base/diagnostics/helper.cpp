#include "helper.h"
#include <boost/format.hpp>

namespace gorc {
namespace diagnostics {
namespace helper {

void feature_not_implemented(report& report, const std::string& visitorname, const std::string& action) {
    report.add_critical_error(visitorname, boost::str(boost::format("feature not implemented: %s") % action));
}

void file_not_found(report& report, const std::string& visitorname, const std::string& filename) {
    report.add_critical_error(visitorname, boost::str(boost::format("file \'%s\' not found") % filename));
}

void could_not_load_file(report& report, const std::string& visitorname, const std::string& filename) {
    report.add_critical_error(visitorname, boost::str(boost::format("file \'%s\' was not loaded") % filename));
}

void unrecognized_input(report& report, const std::string& input, const error_location& location) {
    report.add_error("scanner", boost::str(boost::format("illegal character \'%s\'") % input), location);
}

void eof_in_string_literal(report& report, const error_location& location) {
    report.add_error("scanner", "unexpected end of file in string", location);
}

void unknown_escape_sequence(report& report, const std::string& sequence, const error_location& location) {
    report.add_error("scanner", boost::str(boost::format("unknown escape sequence \'\\%s\'") % sequence), location);
}

void unescaped_newline_in_string(report& report, const error_location& location) {
    report.add_error("scanner", "unescaped newline in string", location);
}

void unknown_type(report& report, const std::string& visitorname, const std::string& type, const error_location& location) {
    report.add_error(visitorname, boost::str(boost::format("unknown type \'%s\'") % type), location);
}

void illegal_assignment(report& report, const std::string& visitorname, const error_location& location) {
    report.add_error(visitorname, "symbol does not accept a default value", location);
}

void type_mismatch(report& report, const std::string& visitorname, const error_location& location) {
    report.add_error(visitorname, "type mismatch", location);
}

void unknown_extension(report& report, const std::string& visitorname, const std::string& extension, const error_location& location) {
    report.add_warning(visitorname, boost::str(boost::format("unknown extension \'%s\'") % extension), location);
}

void illegal_extension_assignment(report& report, const std::string& visitorname, const std::string& extension, const error_location& location) {
    report.add_error(visitorname, boost::str(boost::format("extension \'%s\' does not accept a value") % extension), location);
}

void extension_type_mismatch(report& report, const std::string& visitorname, const std::string& extension, const error_location& location) {
    report.add_error(visitorname, boost::str(boost::format("extension \'%s\' value type mismatch") % extension), location);
}

void symbol_redefinition(report& report, const std::string& visitorname, const std::string& name, const error_location& location) {
    report.add_warning(visitorname, boost::str(boost::format("symbol \'%s\' redefinition") % name), location);
}

void extension_redefinition(report& report, const std::string& visitorname, const std::string& extension, const error_location& location) {
    report.add_warning(visitorname, boost::str(boost::format("extension \'%s\' redefinition") % extension), location);
}

void extension_value_missing(report& report, const std::string& visitorname, const std::string& extension, const error_location& location) {
    report.add_error(visitorname, boost::str(boost::format("extension \'%s\' requires a value") % extension), location);
}

void illegal_extension(report& report, const std::string& visitorname, const std::string& extension, const error_location& location) {
    report.add_warning(visitorname, boost::str(boost::format("symbol does not accept extension \'%s\'") % extension), location);
}

void break_outside_loop(report& report, const std::string& visitorname, const error_location& location) {
    report.add_error(visitorname, "break outside loop scope", location);
}

void label_inside_block(report& report, const std::string& visitorname, const std::string& labelname, const error_location& location) {
    report.add_warning(visitorname, boost::str(boost::format("label \'%s\' inside block expression") % labelname), location);
}

void unknown_verb(report& report, const std::string& visitorname, const std::string& verbname, const error_location& location) {
    report.add_error(visitorname, boost::str(boost::format("unknown verb \'%s\'") % verbname), location);
}

void invalid_argument_count(report& report, const std::string& visitorname, const std::string& verbname, size_t wanted, size_t found, const error_location& location) {
    report.add_error(visitorname, boost::str(boost::format("verb \'%s\' expects %d arguments, found %d") % verbname % wanted % found), location);
}

void undefined_symbol(report& report, const std::string& visitorname, const std::string& symbolname, const error_location& location) {
    report.add_warning(visitorname, boost::str(boost::format("symbol \'%s\' used without being defined") % symbolname), location);
}

void illegal_void_result(report& report, const std::string& visitorname, const error_location& location) {
    report.add_error(visitorname, "value expected, but expression has result type \'void\'", location);
}

void result_not_boolean(report& report, const std::string& visitorname, const error_location& location) {
    report.add_warning(visitorname, "boolean expression expected", location);
}

void call_to_unused_label(report& report, const std::string& visitorname, const std::string& labelname, const error_location& location) {
    report.add_error(visitorname, boost::str(boost::format("label \'%s\' is called but not defined") % labelname), location);
}

void label_redefinition(report& report, const std::string& visitorname, const std::string& labelname, const error_location& location) {
    report.add_error(visitorname, boost::str(boost::format("label \'%s\' already defined") % labelname), location);
}

void missing_export(report& report, const std::string& visitorname, const std::string& labelname, const error_location& location) {
    report.add_warning(visitorname, boost::str(boost::format("message \'%s\' exported but label not defined") % labelname), location);
}

void assign_to_rvalue(report& report, const std::string& visitorname, const error_location& location) {
    report.add_error(visitorname, "assignment target is not an L-value", location);
}

void undefined_array_base(report& report, const std::string& visitorname, const std::string& symbolname, const error_location& location) {
    report.add_error(visitorname, boost::str(boost::format("undefined array base \'%s\'") % symbolname), location);
}

void file_corrupt(report& report, const std::string& visitorname, const error_location& location) {
    report.add_error(visitorname, "file corrupt", location);
}

void expected_number(report& report, const std::string& visitorname, const error_location& location) {
    report.add_error(visitorname, "expected number", location);
}

void expected_punctuator(report& report, const std::string& visitorname, const std::string& found, const std::string& expected, const error_location& location) {
    report.add_error(visitorname, boost::str(boost::format("expected punctuator \'%s\', found \'%s\'") % expected % found), location);
}

void expected_identifier(report& report, const std::string& visitorname, const std::string& found, const std::string& expected, const error_location& location) {
    report.add_error(visitorname, boost::str(boost::format("expected identifier \'%s\', found \'%s\'") % expected % found), location);
}

void expected_filename(report& report, const std::string& visitorname, const error_location& location) {
    report.add_error(visitorname, "expected filename", location);
}

void expected_string(report& report, const std::string& visitorname, const error_location& location) {
    report.add_error(visitorname, "expected string", location);
}

void expected_label(report& report, const std::string& visitorname, const std::string&, const std::string&, const error_location& location) {
    report.add_error(visitorname, "expected label", location);
}

void expected_end_of_file(report& report, const std::string& visitorname, const std::string&, const error_location& location) {
    report.add_error(visitorname, "expected end of file", location);
}

void unexpected_end_of_file_in_string(report& report, const std::string& visitorname, const error_location& location) {
    report.add_error(visitorname, "unexpected end of file in string", location);
}

void unexpected_end_of_line_in_string(report& report, const std::string& visitorname, const error_location& location) {
    report.add_error(visitorname, "unexpected end of line in string", location);
}

void expected(report& report, const std::string& visitorname, const std::string& expected, const error_location& location) {
    report.add_error(visitorname, boost::str(boost::format("expected %s") % expected), location);
}

void unexpected_end_of_file(report& report, const std::string& visitorname, const error_location& location) {
    report.add_error(visitorname, "unexpected end of file", location);
}

void unexpected_end_of_line(report& report, const std::string& visitorname, const error_location& location) {
    report.add_error(visitorname, "unexpected end of line", location);
}

void template_redefinition(report& report, const std::string& visitorname, const std::string& tplname, const error_location& location) {
    report.add_error(visitorname, boost::str(boost::format("template \'%s\' redefinition") % tplname), location);
}

}
}
}
