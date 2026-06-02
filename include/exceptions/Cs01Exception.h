#ifndef NBE_CHAT_SERVER_CS01EXCEPTION_H
#define NBE_CHAT_SERVER_CS01EXCEPTION_H

#include <string>

using std::string;

class Cs01Exception: public std::exception
{
private:
    string type;
    string errorMsg;
    string className;
    //string methodName;
public:
    Cs01Exception(
            string errMsg,
            string type = "None",
            string className = ""
            //,string methodName = ""
    )
    {
        this->errorMsg      = errMsg;
        this->type          = type;

        if (className.size() == 0) {
            this->className = __func__;
        } else {
            this->className     = className;
        }

//        if (methodName.size() == 0) {
//            this->methodName = __func__;
//        } else {
//            this->methodName = methodName;
//        }
    }

    string toString()
    {
        return "Msg: [" + this->errorMsg + "]. Type: [" + this->type + "]. "
               + "Class: [" + this->className + "]."
            //+ " Method: [" + this->methodName + "]."
                ;
    }
};

#endif //NBE_CHAT_SERVER_CS01EXCEPTION_H
