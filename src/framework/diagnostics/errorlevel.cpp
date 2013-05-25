#include <iostream>
#include "errorlevel.h"

std::ostream& operator<<(std::ostream& str, Gorc::Diagnostics::ErrorLevel value) {
	switch(value) {
	case Gorc::Diagnostics::ErrorLevel::Warning:
		return str << "warning";

	case Gorc::Diagnostics::ErrorLevel::Error:
		return str << "error";

	case Gorc::Diagnostics::ErrorLevel::CriticalError:
		return str << "critical error";
	}

	return str;
}
