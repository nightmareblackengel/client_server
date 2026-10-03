
#ifndef NBE_CHAT_BASETRANSMITTER_H
#define NBE_CHAT_BASETRANSMITTER_H

#include <cerrno>
#include "common/AbstractTransmitter.h"
#include "common/interfaces/NonCopyableMovable.h"

class BaseTransmitter:
        public AbstractTransmitter,
        public NonCopyableMovable
{
public:
    int createSocket() override
    {
        //  Создаем TCP-сокет
        // AF_INET - протокол IPv4
        // SOCK_STREAM - потоковый тип сокета (гарантирует доставку TCP)
        // 0           - автоматический выбор протокола (для SOCK_STREAM это всегда TCP)
        int socketId = socket(SERVER_IP_TYPE, SOCK_STREAM, 0);
        if (socketId == DEFAULT_INVALID_DESCRIPTOR) {
            AppException::Throw("Не удалось создать сокет");
        }
        this->bSocket.setId(socketId);

        return 0;
    }

    int sendStringToSocket(int socketId, string& msg) override
    {
        int res = send(socketId, msg.c_str(), msg.length(), 0);
        if (res < 0) {
            AppException::Throw("Отправка сообщения завершилась ошибкой");
        }
        return res;
    }

    string readFromSocket(int socketId, ssize_t& countRes) override
    {
        vector<char> buff(65536);

        // recv() читает данные из сокета клиента.
        // Он тоже блокирующий: ждет, пока клиент что-то пришлет.
        countRes = recv(socketId, buff.data(), buff.size() - 1, 0);
        if (countRes < 0) {
            AppException::Throw("Ошибка при чтении данных (recv)");
        }

        string result(buff.data(), countRes);

        return result;
    }

    int acceptNewClient(int serverSocketId)
    {
        sockaddr_in sockAddr{};
        socklen_t   addrLen = sizeof(sockAddr);

        int clientSocketId = accept(serverSocketId, (struct sockaddr*)&(sockAddr), &(addrLen));
        if (clientSocketId < 0) {
            AppException::Throw("Не удалось принять подключение (accept)");
        }

        return clientSocketId;
    }
};

#endif //NBE_CHAT_BASETRANSMITTER_H
