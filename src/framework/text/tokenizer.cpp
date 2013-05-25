#include "tokenizer.h"
#include "framework/diagnostics/helper.h"

using namespace Gorc::Text;

void Tokenizer::scan() {
	++col;

	if(current == '\n') {
		++line;
		col = 1;
	}

	current = next;
	next = buffer.GetNext();
}

bool Tokenizer::skipWhitespaceStep() {
	if(current == '\0') {
		// EOF in a block of whitespace
		return false;
	}
	else if(report_eol && current == '\n') {
		return false;
	}
	else if(isspace(current)) {
		scan();
		return true;
	}
	else if(current == '#') {
		// Line comment, skip to eol.
		SkipToNextLine();
		return true;
	}
	else {
		return false;
	}
}

void Tokenizer::skipWhitespace() {
	while(skipWhitespaceStep()) {
		// Repeatedly call skipWhitespaceStep() until it returns false.
		// Returning false indicates no more whitespace to skip.
	}

	return;
}

bool Tokenizer::isIdentifierLead(char c) {
	return isalpha(c) || (c == '_');
}

bool Tokenizer::isIdentifierChar(char c) {
	return isalnum(c) || (c == '_');
}

void Tokenizer::readNumericLiteralHexPart(Token& out) {
	while(isxdigit(current)) {
		out.Value.push_back(current);
		scan();
	}
}

void Tokenizer::readNumericLiteralIntegerPart(Token& out) {
	while(isdigit(current)) {
		out.Value.push_back(current);
		scan();
	}
}

void Tokenizer::readNumericLiteral(Token& out) {
	// Check for sign:
	if(current == '-') {
		out.Value.push_back(current);
		scan();
	}

	if(current == '0' && (next == 'x' || next == 'X')) {
		out.Type = TokenType::HexInteger;

		out.Value.push_back(current);
		out.Value.push_back(next);

		scan();
		scan();

		readNumericLiteralHexPart(out);
		return;
	}

	out.Type = TokenType::Integer;

	readNumericLiteralIntegerPart(out);

	if(current == '.' && isdigit(next)) {
		out.Value.push_back(current);
		scan();

		readNumericLiteralIntegerPart(out);

		out.Type = TokenType::Float;
	}

	if(current == 'e' || current == 'E') {
		out.Value.push_back(current);
		scan();

		if(current == '+' || current == '-') {
			out.Value.push_back(current);
			scan();
		}

		readNumericLiteralIntegerPart(out);
		out.Type = TokenType::Float;
	}
}

void Tokenizer::readIdentifier(Token& out) {
	if(isIdentifierLead(current)) {
		// Valid identifier
		out.Type = TokenType::Identifier;

		do {
			out.Value.push_back(current);
			scan();
		} while(isIdentifierChar(current));
	}
}

void Tokenizer::readStringLiteral(Token& out) {
	while(true) {
		scan();

		if(current == '\0') {
			out.Location.last_line = line;
			out.Location.last_column = col;
			Diagnostics::Helper::UnexpectedEndOfFileInString(ErrorReport, "tokenizer", out.Location);
			out.Type = TokenType::Invalid;
			return;
		}
		else if(current == '\n') {
			out.Location.last_line = line;
			out.Location.last_column = col;
			Diagnostics::Helper::UnexpectedEndOfLineInString(ErrorReport, "tokenizer", out.Location);
			out.Type = TokenType::Invalid;
			return;
		}
		else if(current == '\"') {
			out.Type = TokenType::String;
			scan();
			return;
		}
		else if(current == '\\') {
			// Escape sequence.
			scan();

			switch(current) {
			case '\n':
				// Escaped newline
				break;

			case '\'':
			case '\"':
			case '\\':
				out.Value.push_back(current);
				break;

			case 'n':
				out.Value.push_back('\n');
				break;

			case 't':
				out.Value.push_back('\t');
				break;

			default:
				out.Type = TokenType::Invalid;
				return;
			}
		}
		else {
			out.Value.push_back(current);
		}
	}
}

Tokenizer::Tokenizer(Source& stream, Diagnostics::Report& errorReport)
	: ErrorReport(errorReport), buffer(stream), report_eol(false) {
	line = 1;
	col = 1;
	current = buffer.GetNext();
	next = buffer.GetNext();
}

void Tokenizer::SkipToNextLine() {
	while(current != '\n' && current != '\0') {
		scan();
	}
}

void Tokenizer::GetToken(Token& out) {
	skipWhitespace();

	out.Value.clear();
	out.Location.first_line = line;
	out.Location.first_column = col;

	if(current == '\0') {
		// Stream has reached end of file.
		out.Type = TokenType::EndOfFile;
	}
	else if(current == '\n') {
		out.Type = TokenType::EndOfLine;
		scan();
	}
	else if(current == '\"') {
		readStringLiteral(out);
	}
	else if(isIdentifierLead(current)) {
		readIdentifier(out);
	}
	else if(isdigit(current)) {
		readNumericLiteral(out);
	}
	else if(ispunct(current)) {
		if(current == '-' && isdigit(next)) {
			readNumericLiteral(out);
		}
		else {
			out.Value.push_back(current);
			out.Type = TokenType::Punctuator;
		}

		scan();
	}
	else {
		out.Type = TokenType::Invalid;
	}

	out.Location.last_line = line;
	out.Location.last_column = col;
}

void Tokenizer::GetDelimitedString(Token& out, char delimiter) {
	skipWhitespace();

	out.Value.clear();
	out.Location.filename = GetFilename().c_str();
	out.Location.first_line = line;
	out.Location.first_column = col;

	while(current != delimiter) {
		out.Value.push_back(current);
		scan();
	}

	out.Type = TokenType::String;
	out.Location.last_line = line;
	out.Location.last_column = col;

	return;
}

void Tokenizer::AssertIdentifier(const std::string& id) {
	GetToken(internalToken);

	if(internalToken.Type != TokenType::Identifier || internalToken.Value != id) {
		Diagnostics::Helper::ExpectedIdentifier(ErrorReport, "tokenizer", internalToken.Value, id, internalToken.Location);
		throw TokenizerAssertionException();
	}
}

void Tokenizer::AssertPunctuator(const std::string& punc) {
	GetToken(internalToken);

	if(internalToken.Type != TokenType::Punctuator || internalToken.Value != punc) {
		Diagnostics::Helper::ExpectedPunctuator(ErrorReport, "tokenizer", internalToken.Value, punc, internalToken.Location);
		throw TokenizerAssertionException();
	}
}

void Tokenizer::AssertLabel(const std::string& label) {
	GetToken(internalToken);

	if(internalToken.Type != TokenType::Identifier || internalToken.Value != label) {
		Diagnostics::Helper::ExpectedLabel(ErrorReport, "tokenizer", internalToken.Value, label, internalToken.Location);
		throw TokenizerAssertionException();
	}

	GetToken(internalToken);

	if(internalToken.Type != TokenType::Punctuator || internalToken.Value != ":") {
		Diagnostics::Helper::ExpectedLabel(ErrorReport, "tokenizer", internalToken.Value, ":", internalToken.Location);
		throw TokenizerAssertionException();
	}
}
