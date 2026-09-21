
#ifndef NBE_CHAT_BLOCKINGCLIENT_H
#define NBE_CHAT_BLOCKINGCLIENT_H

#include "BlockingTransmitter.h"

class BlockingClient: public BlockingTransmitter
{
public:
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
