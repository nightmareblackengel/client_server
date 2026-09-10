#ifndef NBE_CHAT_SERVER_CS01EXCEPTION_H
#define NBE_CHAT_SERVER_CS01EXCEPTION_H

#include <string>
#include <string_view>
#include <source_location>

using std::string;
using std::string_view;

constexpr const string_view CS01_CLIENT_TYPE = "Client";
constexpr const string_view CS01_SERVER_TYPE = "Server";

class Cs01Exception: public std::exception
{
protected:
    string type;
    string errorMsg;
    string className;
    int line;
    string fileName;
    string functionName;
public:
    Cs01Exception(
            string errMsg,
            string type = "None",
            string className = "",
            std::source_location loc = std::source_location::current()
    )
    {
        this->errorMsg      = errMsg;
        this->type          = type;

        if (className.length() == 0) {
            this->className = __func__ ;
        } else {
            this->className     = className;
        }

        this->fileName = loc.file_name();
        this->functionName = loc.function_name();
        this->line = loc.line();
    }

    string toString()
    {
        return this->errorMsg + ". Type: [" + this->type + "]. "
               + "Class: [" + this->className + "].\n"
               + "FuncName:" + this->functionName
               + ". FileName: [" + this->fileName + "]:" + std::to_string(this->line)
               + ".";
    }

    const char* what() const noexcept override
    {
        return this->errorMsg.c_str();
    }
};

#endif //NBE_CHAT_SERVER_CS01EXCEPTION_H
