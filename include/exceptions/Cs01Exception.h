#ifndef NBE_CHAT_SERVER_CS01EXCEPTION_H
#define NBE_CHAT_SERVER_CS01EXCEPTION_H

#include <string>

using std::string;

const string CS01_CLIENT_TYPE = "Client";
const string CS01_SERVER_TYPE = "Server";

class Cs01Exception: public std::exception
{
protected:
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
        return " " + this->errorMsg + ". Type: [" + this->type + "]. "
               + "Class: [" + this->className + "]."
            //+ " Method: [" + this->methodName + "]."
                ;
    }
};

class Cs01ClientException: public Cs01Exception
{
public:
    Cs01ClientException(string errMsg):
            Cs01Exception(errMsg, CS01_CLIENT_TYPE, __func__ )
    {

    }
};

class Cs01ServerException: public Cs01Exception
{
public:
    Cs01ServerException(string errMsg):
            Cs01Exception(errMsg, CS01_CLIENT_TYPE, __func__ )
    {

    }
};

#endif //NBE_CHAT_SERVER_CS01EXCEPTION_H
