
#ifndef NBE_CHAT_NBEEXCEPTION_H
#define NBE_CHAT_NBEEXCEPTION_H

#include <string>
#include <source_location>

using std::string;

class TransmitterException: public std::exception
{
protected:
    string sourceType;
    string errorMessage;
    string className;
    int line;
    string fileName;
    string functionName;
public:
    TransmitterException(
            string _errMsg,
            string &_type,
            string &_className,
            std::source_location _loc = std::source_location::current()
    ): sourceType(_type), errorMessage(_errMsg)
    {
        if (_className.length() == 0) {
            this->className = __func__ ;
        } else {
            this->className = _className;
        }

        this->fileName = _loc.file_name();
        this->functionName = _loc.function_name();
        this->line = _loc.line();
    }

    string toString()
    {
        return "{Nbe} Error message:" + this->errorMessage + ". Type: [" + this->sourceType + "]. "
               + "Class: [" + this->className + "].\n"
               + "FuncName:" + this->functionName
               + ". FileName: [" + this->fileName + "]:" + std::to_string(this->line)
               + ".";
    }

    const char* what() const noexcept override
    {
        return this->errorMessage.c_str();
    }
};

#endif //NBE_CHAT_NBEEXCEPTION_H
