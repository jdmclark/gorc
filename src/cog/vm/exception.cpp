#include "exception.h"

const char* Gorc::Cog::VM::CodeBufferOverflowException::what() const throw() {
	return "code buffer overflow";
}
