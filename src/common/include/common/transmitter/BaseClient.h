
#ifndef NBE_CHAT_BASECLIENT_H
#define NBE_CHAT_BASECLIENT_H

#include "BaseTransmitter.h"

class BaseClient: public BaseTransmitter
{
public:
    BaseClient(): BaseTransmitter()
    {
        AppException::type = CS01_CLIENT_TYPE;
    }

    virtual int connectToSocket()
    {
        // Настройка структуры адреса сервера
        sockaddr_in address = this->getConfiguredAddress();

        if (connect(this->getSocketId(), (struct sockaddr*) &address, sizeof(address)) == -1) {
            AppException::Throw("Не удалось подключиться к серверу");
        }

        return 0;
    }
};

#endif //NBE_CHAT_BASECLIENT_H
