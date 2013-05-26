#pragma once

#include "report.h"

namespace Gorc {
namespace Diagnostics {
namespace Helper {

// Compiler internal errors
void FeatureNotImplemented(Report& report, const std::string& visitorname, const std::string& action);
void FileNotFound(Report& report, const std::string& visitorname, const std::string& filename);

// Compiler lexical errors
void UnrecognizedInput(Report& report, const std::string& input, const ErrorLocation& location);
void EofInStringLiteral(Report& Report, const ErrorLocation& location);
void UnknownEscapeSequence(Report& report, const std::string& sequence, const ErrorLocation& location);
void UnescapedNewlineInString(Report& report, const ErrorLocation& location);

// Compiler semantic errors
void UnknownType(Report& report, const std::string& visitorname, const std::string& type, const ErrorLocation& location);
void IllegalAssignment(Report& report, const std::string& visitorname, const ErrorLocation& location);
void TypeMismatch(Report& report, const std::string& visitorname, const ErrorLocation& location);
void UnknownExtension(Report& report, const std::string& visitorname, const std::string& extension, const ErrorLocation& location);
void IllegalExtensionAssignment(Report& report, const std::string& visitorname, const std::string& extension, const ErrorLocation& location);
void ExtensionTypeMismatch(Report& report, const std::string& visitorname, const std::string& extension, const ErrorLocation& location);
void SymbolRedefinition(Report& report, const std::string& visitorname, const std::string& name, const ErrorLocation& location);
void ExtensionRedefinition(Report& report, const std::string& visitorname, const std::string& extension, const ErrorLocation& location);
void ExtensionValueMissing(Report& report, const std::string& visitorname, const std::string& extension, const ErrorLocation& location);
void IllegalExtension(Report& Report, const std::string& visitorname, const std::string& extension, const ErrorLocation& location);
void BreakOutsideLoop(Report& Report, const std::string& visitorname, const ErrorLocation& location);
void LabelInsideBlock(Report& Report, const std::string& visitorname, const std::string& labelname, const ErrorLocation& location);
void UnknownVerb(Report& Report, const std::string& visitorname, const std::string& verbname, const ErrorLocation& location);
void InvalidArgumentCount(Report& Report, const std::string& visitorname, const std::string& verbname, int wanted, int found, const ErrorLocation& location);
void UndefinedSymbol(Report& Report, const std::string& visitorname, const std::string& symbolname, const ErrorLocation& location);
void IllegalVoidResult(Report& Report, const std::string& visitorname, const ErrorLocation& location);
void ResultNotBoolean(Report& Report, const std::string& visitorname, const ErrorLocation& location);
void CallToUnusedLabel(Report& Report, const std::string& visitorname, const std::string& labelname, const ErrorLocation& location);
void LabelRedefinition(Report& Report, const std::string& visitorname, const std::string& labelname, const ErrorLocation& location);
void MissingExport(Report& Report, const std::string& visitorname, const std::string& labelname, const ErrorLocation& location);
void AssignToRValue(Report& Report, const std::string& visitorname, const ErrorLocation& location);
void UndefinedArrayBase(Report& Report, const std::string& visitorname, const std::string& symbolname, const ErrorLocation& location);

// Other file errors
void FileCorrupt(Report& Report, const std::string& visitorname, const ErrorLocation& location);
void ExpectedNumber(Report& Report, const std::string& visitorname, const ErrorLocation& location);
void ExpectedPunctuator(Report& Report, const std::string& visitorname, const std::string& found, const std::string& expected, const ErrorLocation& location);
void ExpectedIdentifier(Report& Report, const std::string& visitorname, const std::string& found, const std::string& expected, const ErrorLocation& location);
void ExpectedFilename(Report& Report, const std::string& visitorname, const ErrorLocation& location);
void ExpectedString(Report& Report, const std::string& visitorname, const ErrorLocation& location);
void ExpectedLabel(Report& Report, const std::string& visitorname, const std::string& found, const std::string& expected, const ErrorLocation& location);
void ExpectedEndOfFile(Report& Report, const std::string& visitorname, const std::string& found, const ErrorLocation& location);
void UnexpectedEndOfFileInString(Report& Report, const std::string& visitorname, const ErrorLocation& location);
void UnexpectedEndOfLineInString(Report& Report, const std::string& visitorname, const ErrorLocation& location);
void Expected(Report& Report, const std::string& visitorname, const std::string& expected, const ErrorLocation& location);
void UnexpectedEndOfFile(Report& Report, const std::string& visitorname, const ErrorLocation& location);
void UnexpectedEndOfLine(Report& Report, const std::string& visitorname, const ErrorLocation& location);
void TemplateRedefinition(Report& Report, const std::string& visitorname, const std::string& tplname, const ErrorLocation& location);

}
}
}
