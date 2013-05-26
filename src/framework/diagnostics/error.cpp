#include "error.h"

#include <sstream>
#include <iomanip>

using namespace Gorc::Diagnostics;

Error::Error(ErrorLevel level, const std::string& stage, const std::string& reason,
		const ErrorLocation& location)
	: Level(level), Stage(stage), Reason(reason), Location(location) {
	return;
}

std::string FormatError(const Error& error) {
	std::stringstream ss;

	ss << error.Level << ": " << error.Reason << " [" << error.Stage << "]";
	return ss.str();
}

std::string FormatLineError(const Error& error) {
	std::stringstream ss;

	ss << error.Location.filename << "(" << error.Location.first_line << "," << error.Location.first_column << "): "
			<< error.Level << ": " << error.Reason << " [" << error.Stage << "]";
	return ss.str();
}

Error::operator std::string() const {
	switch(Level) {
	case ErrorLevel::CriticalError:
		return FormatError(*this);

	case ErrorLevel::Error:
	case ErrorLevel::Warning:
		return FormatLineError(*this);
	}

	return std::string();
}
