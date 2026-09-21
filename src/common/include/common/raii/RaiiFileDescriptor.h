#ifndef NBE_CHAT_SERVER_ISOCKET_H
#define NBE_CHAT_SERVER_ISOCKET_H

#include <iostream>
#include <unistd.h>
#include "common/bootstrap.h"

using std::cout;
using std::endl;

class RaiiFileDescriptor
{
protected:
    int identificator;
public:
    RaiiFileDescriptor()
    {
        this->identificator = DEFAULT_INVALID_DESCRIPTOR;
    }

    virtual ~RaiiFileDescriptor()
    {
        this->Close();
    }

    RaiiFileDescriptor(const RaiiFileDescriptor&) = delete;
    RaiiFileDescriptor& operator=(const RaiiFileDescriptor&) = delete;

    RaiiFileDescriptor(RaiiFileDescriptor&&) = delete;
    RaiiFileDescriptor& operator=(RaiiFileDescriptor&&) = delete;

    int getId()
    {
        return this->identificator;
    }
    void setId(int _socketId)
    {
        this->identificator = _socketId;
    }

    int Close()
    {
        cout << "Закрываем дескриптор [" << this->identificator << "]..." << endl;
        if (this->identificator == DEFAULT_INVALID_DESCRIPTOR) {
            return 0;
        }

        int closeRes = close(this->identificator);
        if (closeRes == 0) {
            cout << "Дескриптор закрыт [" << this->identificator << "]" << endl;
        } else {
            cout << IoTextColor::RED << "Дескриптор незакрыт [" << this->identificator << "]. Код ошибки: " << closeRes  << IoTextColor::DEFAULT << endl;
        }
        this->identificator = DEFAULT_INVALID_DESCRIPTOR;

        return closeRes;
    }
};

#endif //NBE_CHAT_SERVER_ISOCKET_H
