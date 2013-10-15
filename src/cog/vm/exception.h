#pragma once

#include "framework/exception.h"

namespace gorc {
namespace cog {
namespace vm {

class virtual_machine_exception : public exception { };

class code_buffer_overflow_exception : public virtual_machine_exception {
public:
	virtual const char* what() const throw() override;
};

}
}
}
