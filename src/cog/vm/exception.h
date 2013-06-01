#pragma once

#include "framework/exception.h"

namespace Gorc {
namespace Cog {
namespace VM {

class VirtualMachineException : public Exception { };

class CodeBufferOverflowException : public VirtualMachineException {
public:
	virtual const char* what() const throw() override;
};

}
}
}
