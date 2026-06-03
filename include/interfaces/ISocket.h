#ifndef NBE_CHAT_SERVER_ISOCKET_H
#define NBE_CHAT_SERVER_ISOCKET_H

#include <iostream>
#include <unistd.h>
#include "bootstrap.h"

using std::cout;
using std::endl;

class ISocket
{
protected:
    int socketId;

    ISocket()
    {
        this->socketId = DEFAULT_INVALID_DESCRIPTOR;
    }

    ~ISocket()
    {
        this->Close();
    }

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
        if (this->socketId == DEFAULT_INVALID_DESCRIPTOR) {
            return 0;
        }

        int closeRes = close(this->socketId);
        if (closeRes == 0) {
            cout << "resource закрыт" << endl;
        } else {
            cout << "resource незакрыт. Код ошибки: " << closeRes << endl;
        }
        this->socketId = DEFAULT_INVALID_DESCRIPTOR;

        return closeRes;
    }
};

#endif //NBE_CHAT_SERVER_ISOCKET_H
