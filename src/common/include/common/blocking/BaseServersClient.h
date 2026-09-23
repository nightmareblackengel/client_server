
#ifndef NBE_CHAT_BASESERVERSCLIENT_H
#define NBE_CHAT_BASESERVERSCLIENT_H

#include "BaseTransmitter.h"

class BaseServersClient: public BaseTransmitter
{
public:
    int acceptFromServer(int serverSocket)
    {
        sockaddr_in sockAddr{};
        socklen_t   addrLen = sizeof(sockAddr);

        // accept блокирует поток, пока кто-то не подключится
        int socketId = accept(serverSocket, (struct sockaddr*)&(sockAddr), &(addrLen));
        if (socketId < 0) {
            this->throwException("Не удалось принять подключение (accept)");
        }
        this->bSocket.setId(socketId);
        cout << "ServersClient успешно подключился! Дескриптор клиента: " << socketId << endl;
        return socketId;
    }
};

#endif //NBE_CHAT_BASESERVERSCLIENT_H
