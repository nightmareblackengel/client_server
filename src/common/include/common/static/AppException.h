#ifndef NBE_CHAT_APPEXCEPTION_H
#define NBE_CHAT_APPEXCEPTION_H

#include <string>
#include <source_location>
#include "common/exceptions/TransmitterException.h"

class AppException
{
public:
    static string type;

    static void Throw(const string &msg, std::source_location loc = std::source_location::current())
    {
        string func1 = __func__;
        throw TransmitterException(msg, AppException::type, func1, loc);
    }
};

string AppException::type;

#endif //NBE_CHAT_APPEXCEPTION_H
