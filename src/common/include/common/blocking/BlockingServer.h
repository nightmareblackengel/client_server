
#ifndef NBE_CHAT_BLOCKINGSERVER_H
#define NBE_CHAT_BLOCKINGSERVER_H

#include "BlockingTransmitter.h"

class BlockingServer: public BlockingTransmitter
{
public:
    int bindSocket() override
    {
        sockaddr_in address = this->getConfiguredAddress();
        // Проблема быстрого перезапуска сервера
        int opt = 1;
        setsockopt(this->bSocket.getId(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // Привязываем сокет к адресу и порту (bind)
        int bindRes = bind(this->bSocket.getId(), (struct sockaddr*)&address, sizeof(address));
        if (bindRes < 0) {
            this->throwException("Привязка сокета (bind) завершилась ошибкой");
        }

        return bindRes;
    }

    int listenSocket(int requestSize = 10) override
    {
        // Переводим сокет в режим прослушивания (listen)
        // 10 - это размер очереди "недообработанных" подключений (backlog)
        int listenRes = listen(this->bSocket.getId(), requestSize);
        if (listenRes < 0) {
            this->throwException("Перевод сокета в режим listen завершился ошибкой");
        }

        return listenRes;
    }
};

#endif //NBE_CHAT_BLOCKINGSERVER_H
