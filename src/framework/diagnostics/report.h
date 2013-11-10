#pragma once

#include "error_location.h"

#include <string>

namespace gorc {
namespace diagnostics {

class report {
public:
	virtual ~report();

	virtual void add_error(const std::string& stage, const std::string& reason, const error_location& location) = 0;
	virtual void add_warning(const std::string& stage, const std::string& reason, const error_location& location) = 0;
	virtual void add_critical_error(const std::string& stage, const std::string& reason) = 0;

	virtual unsigned int get_error_count() const = 0;
	virtual unsigned int get_warning_count() const = 0;
};

}
}
