#include "helper.h"
#include <boost/format.hpp>

namespace Gorc {
namespace Diagnostics {
namespace Helper {

void FeatureNotImplemented(Report& report, const std::string& visitorname, const std::string& action) {
	report.AddCriticalError(visitorname, boost::str(boost::format("feature not implemented: %s") % action));
}

void FileNotFound(Report& report, const std::string& visitorname, const std::string& filename) {
	report.AddCriticalError(visitorname, boost::str(boost::format("file \'%s\' not found") % filename));
}

void CouldNotLoadFile(Report& report, const std::string& visitorname, const std::string& filename) {
	report.AddCriticalError(visitorname, boost::str(boost::format("file \'%s\' was not loaded") % filename));
}

void UnrecognizedInput(Report& report, const std::string& input, const ErrorLocation& location) {
	report.AddError("scanner", boost::str(boost::format("illegal character \'%s\'") % input), location);
}

void EofInStringLiteral(Report& report, const ErrorLocation& location) {
	report.AddError("scanner", "unexpected end of file in string", location);
}

void UnknownEscapeSequence(Report& report, const std::string& sequence, const ErrorLocation& location) {
	report.AddError("scanner", boost::str(boost::format("unknown escape sequence \'\\%s\'") % sequence), location);
}

void UnescapedNewlineInString(Report& report, const ErrorLocation& location) {
	report.AddError("scanner", "unescaped newline in string", location);
}

void UnknownType(Report& report, const std::string& visitorname, const std::string& type, const ErrorLocation& location) {
	report.AddError(visitorname, boost::str(boost::format("unknown type \'%s\'") % type), location);
}

void IllegalAssignment(Report& report, const std::string& visitorname, const ErrorLocation& location) {
	report.AddError(visitorname, "symbol does not accept a default value", location);
}

void TypeMismatch(Report& report, const std::string& visitorname, const ErrorLocation& location) {
	report.AddError(visitorname, "type mismatch", location);
}

void UnknownExtension(Report& report, const std::string& visitorname, const std::string& extension, const ErrorLocation& location) {
	report.AddWarning(visitorname, boost::str(boost::format("unknown extension \'%s\'") % extension), location);
}

void IllegalExtensionAssignment(Report& report, const std::string& visitorname, const std::string& extension, const ErrorLocation& location) {
	report.AddError(visitorname, boost::str(boost::format("extension \'%s\' does not accept a value") % extension), location);
}

void ExtensionTypeMismatch(Report& report, const std::string& visitorname, const std::string& extension, const ErrorLocation& location) {
	report.AddError(visitorname, boost::str(boost::format("extension \'%s\' value type mismatch") % extension), location);
}

void SymbolRedefinition(Report& report, const std::string& visitorname, const std::string& name, const ErrorLocation& location) {
	report.AddWarning(visitorname, boost::str(boost::format("symbol \'%s\' redefinition") % name), location);
}

void ExtensionRedefinition(Report& report, const std::string& visitorname, const std::string& extension, const ErrorLocation& location) {
	report.AddWarning(visitorname, boost::str(boost::format("extension \'%s\' redefinition") % extension), location);
}

void ExtensionValueMissing(Report& report, const std::string& visitorname, const std::string& extension, const ErrorLocation& location) {
	report.AddError(visitorname, boost::str(boost::format("extension \'%s\' requires a value") % extension), location);
}

void IllegalExtension(Report& report, const std::string& visitorname, const std::string& extension, const ErrorLocation& location) {
	report.AddWarning(visitorname, boost::str(boost::format("symbol does not accept extension \'%s\'") % extension), location);
}

void BreakOutsideLoop(Report& report, const std::string& visitorname, const ErrorLocation& location) {
	report.AddError(visitorname, "break outside loop scope", location);
}

void LabelInsideBlock(Report& report, const std::string& visitorname, const std::string& labelname, const ErrorLocation& location) {
	report.AddWarning(visitorname, boost::str(boost::format("label \'%s\' inside block expression") % labelname), location);
}

void UnknownVerb(Report& report, const std::string& visitorname, const std::string& verbname, const ErrorLocation& location) {
	report.AddError(visitorname, boost::str(boost::format("unknown verb \'%s\'") % verbname), location);
}

void InvalidArgumentCount(Report& report, const std::string& visitorname, const std::string& verbname, int wanted, int found, const ErrorLocation& location) {
	report.AddError(visitorname, boost::str(boost::format("verb \'%s\' expects %d arguments, found %d") % verbname % wanted % found), location);
}

void UndefinedSymbol(Report& report, const std::string& visitorname, const std::string& symbolname, const ErrorLocation& location) {
	report.AddWarning(visitorname, boost::str(boost::format("symbol \'%s\' used without being defined") % symbolname), location);
}

void IllegalVoidResult(Report& report, const std::string& visitorname, const ErrorLocation& location) {
	report.AddError(visitorname, "value expected, but expression has result type \'void\'", location);
}

void ResultNotBoolean(Report& report, const std::string& visitorname, const ErrorLocation& location) {
	report.AddWarning(visitorname, "boolean expression expected", location);
}

void CallToUnusedLabel(Report& report, const std::string& visitorname, const std::string& labelname, const ErrorLocation& location) {
	report.AddError(visitorname, boost::str(boost::format("label \'%s\' is called but not defined") % labelname), location);
}

void LabelRedefinition(Report& report, const std::string& visitorname, const std::string& labelname, const ErrorLocation& location) {
	report.AddError(visitorname, boost::str(boost::format("label \'%s\' already defined") % labelname), location);
}

void MissingExport(Report& report, const std::string& visitorname, const std::string& labelname, const ErrorLocation& location) {
	report.AddWarning(visitorname, boost::str(boost::format("message \'%s\' exported but label not defined") % labelname), location);
}

void AssignToRValue(Report& report, const std::string& visitorname, const ErrorLocation& location) {
	report.AddError(visitorname, "assignment target is not an L-value", location);
}

void UndefinedArrayBase(Report& report, const std::string& visitorname, const std::string& symbolname, const ErrorLocation& location) {
	report.AddError(visitorname, boost::str(boost::format("undefined array base \'%s\'") % symbolname), location);
}

void FileCorrupt(Report& report, const std::string& visitorname, const ErrorLocation& location) {
	report.AddError(visitorname, "file corrupt", location);
}

void ExpectedNumber(Report& report, const std::string& visitorname, const ErrorLocation& location) {
	report.AddError(visitorname, "expected number", location);
}

void ExpectedPunctuator(Report& report, const std::string& visitorname, const std::string& found, const std::string& expected, const ErrorLocation& location) {
	report.AddError(visitorname, boost::str(boost::format("expected punctuator \'%s\', found \'%s\'") % expected % found), location);
}

void ExpectedIdentifier(Report& report, const std::string& visitorname, const std::string& found, const std::string& expected, const ErrorLocation& location) {
	report.AddError(visitorname, boost::str(boost::format("expected identifier \'%s\', found \'%s\'") % expected % found), location);
}

void ExpectedFilename(Report& report, const std::string& visitorname, const ErrorLocation& location) {
	report.AddError(visitorname, "expected filename", location);
}

void ExpectedString(Report& report, const std::string& visitorname, const ErrorLocation& location) {
	report.AddError(visitorname, "expected string", location);
}

void ExpectedLabel(Report& report, const std::string& visitorname, const std::string& found, const std::string& expected, const ErrorLocation& location) {
	report.AddError(visitorname, "expected label", location);
}

void ExpectedEndOfFile(Report& report, const std::string& visitorname, const std::string& found, const ErrorLocation& location) {
	report.AddError(visitorname, "expected end of file", location);
}

void UnexpectedEndOfFileInString(Report& report, const std::string& visitorname, const ErrorLocation& location) {
	report.AddError(visitorname, "unexpected end of file in string", location);
}

void UnexpectedEndOfLineInString(Report& report, const std::string& visitorname, const ErrorLocation& location) {
	report.AddError(visitorname, "unexpected end of line in string", location);
}

void Expected(Report& report, const std::string& visitorname, const std::string& expected, const ErrorLocation& location) {
	report.AddError(visitorname, boost::str(boost::format("expected %s") % expected), location);
}

void UnexpectedEndOfFile(Report& report, const std::string& visitorname, const ErrorLocation& location) {
	report.AddError(visitorname, "unexpected end of file", location);
}

void UnexpectedEndOfLine(Report& report, const std::string& visitorname, const ErrorLocation& location) {
	report.AddError(visitorname, "unexpected end of line", location);
}

void TemplateRedefinition(Report& report, const std::string& visitorname, const std::string& tplname, const ErrorLocation& location) {
	report.AddError(visitorname, boost::str(boost::format("template \'%s\' redefinition") % tplname), location);
}

}
}
}
