#ifndef NBE_CHAT_SERVER_ISOCKET_H
#define NBE_CHAT_SERVER_ISOCKET_H

#include <iostream>
#include <unistd.h>
#include "../bootstrap.h"

using std::cout;
using std::endl;

class ISocket
{
protected:
    int socketId;
public:
    ISocket()
    {
        this->socketId = DEFAULT_INVALID_DESCRIPTOR;
    }

    virtual ~ISocket()
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
        cout << "Закрываем порт [" << this->socketId << "]..." << endl;
        if (this->socketId == DEFAULT_INVALID_DESCRIPTOR) {
            return 0;
        }

        int closeRes = close(this->socketId);
        if (closeRes == 0) {
            cout << "Порт закрыт [" << this->socketId << "]" << endl;
        } else {
            cout << IoTextColor::RED << "Порт незакрыт [" << this->socketId << "]. Код ошибки: " << closeRes  << IoTextColor::DEFAULT << endl;
        }
        this->socketId = DEFAULT_INVALID_DESCRIPTOR;

        return closeRes;
    }
};

#endif //NBE_CHAT_SERVER_ISOCKET_H
