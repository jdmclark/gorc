#pragma once

#include "framework/exception.h"

namespace Gorc {
namespace Cog {
namespace VM {

class CodeBufferOverflowException : public Exception {
public:
	virtual const char* what() const throw() override;
};

}
}
}
