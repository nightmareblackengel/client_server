
#ifndef NBE_CHAT_BLOCKINGCLIENT_H
#define NBE_CHAT_BLOCKINGCLIENT_H

#include "BlockingTransmitter.h"

class BlockingClient: public BlockingTransmitter
{
public:
    BlockingClient(): BlockingTransmitter()
    {
        this->errorType = CS01_CLIENT_TYPE;
    }

    virtual int connectToSocket()
    {
        // Настройка структуры адреса сервера
        sockaddr_in address = this->getConfiguredAddress();

        if (connect(this->bSocket.getId(), (struct sockaddr*) &address, sizeof(address)) == -1) {
            this->throwException("Не удалось подключиться к серверу");
        }

        return 0;
    }
};

#endif //NBE_CHAT_BLOCKINGCLIENT_H
