
#include "RuntimeException.h"
#include "StringUtil.h"

namespace common { namespace utility {

    RuntimeException::RuntimeException(const char *format, ...) {
        va_list args;
        va_start(args, format);

        message="RuntimeException: "+strvsprintf(format, args);

        va_end(args);
    }

    const char* RuntimeException::what() const throw() {
        return message.c_str();
    }

}}
