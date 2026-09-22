
#ifndef NBE_CHAT_BLOCKINGTRANSMITTER_H
#define NBE_CHAT_BLOCKINGTRANSMITTER_H

#include "common/BaseTransmitter.h"
#include "common/interfaces/NonCopyableMovable.h"

class BlockingTransmitter:
        public BaseTransmitter,
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
            this->throwException("Не удалось создать сокет");
        }
        this->bSocket.setId(socketId);

        return 0;
    }

    int sendStringToSocket(int socketId, string& msg) override
    {
        int res = send(socketId, msg.c_str(), msg.length(), 0);
        if (res < 0) {
            this->throwException("Отправка сообщения завершилась ошибкой");
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
            this->throwException("Ошибка при чтении данных (recv)");
        }

        string result(buff.data(), countRes);

        return result;
    }
};

#endif //NBE_CHAT_BLOCKINGTRANSMITTER_H
