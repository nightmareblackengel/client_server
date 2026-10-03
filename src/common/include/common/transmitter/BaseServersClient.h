
#ifndef NBE_CHAT_BASESERVERSCLIENT_H
#define NBE_CHAT_BASESERVERSCLIENT_H

#include "BaseTransmitter.h"

class BaseServersClient: public BaseTransmitter
{
public:
    int acceptFromServer(int serverSocket)
    {
        int socketId = this->acceptNewClient(serverSocket);

        this->bSocket.setId(socketId);
        cout << "ServersClient успешно подключился! Дескриптор клиента: " << socketId << endl;
        return socketId;
    }
};

#endif //NBE_CHAT_BASESERVERSCLIENT_H
