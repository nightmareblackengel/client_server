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
    int socketId;
public:
    RaiiFileDescriptor()
    {
        this->socketId = DEFAULT_INVALID_DESCRIPTOR;
    }

    virtual ~RaiiFileDescriptor()
    {
        this->Close();
    }

    RaiiFileDescriptor(const RaiiFileDescriptor&) = delete;
    RaiiFileDescriptor& operator=(const RaiiFileDescriptor&) = delete;

    RaiiFileDescriptor(RaiiFileDescriptor&&) = delete;
    RaiiFileDescriptor& operator=(RaiiFileDescriptor&&) = delete;

    int getSocketId()
    {
        return this->socketId;
    }
    void setSocketId(int _socketId)
    {
        this->socketId = _socketId;
    }

    int Close()
    {
        cout << "Закрываем дескриптор [" << this->socketId << "]..." << endl;
        if (this->socketId == DEFAULT_INVALID_DESCRIPTOR) {
            return 0;
        }

        int closeRes = close(this->socketId);
        if (closeRes == 0) {
            cout << "Дескриптор закрыт [" << this->socketId << "]" << endl;
        } else {
            cout << IoTextColor::RED << "Дескриптор незакрыт [" << this->socketId << "]. Код ошибки: " << closeRes  << IoTextColor::DEFAULT << endl;
        }
        this->socketId = DEFAULT_INVALID_DESCRIPTOR;

        return closeRes;
    }
};

#endif //NBE_CHAT_SERVER_ISOCKET_H
