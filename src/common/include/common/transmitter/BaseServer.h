
#ifndef NBE_CHAT_BASEGSERVER_H
#define NBE_CHAT_BASEGSERVER_H

#include "BaseTransmitter.h"

class BaseServer: public BaseTransmitter
{
public:
    BaseServer(): BaseTransmitter()
    {

    }

    int bindSocket()
    {
        sockaddr_in address = this->getConfiguredAddress();
        // Проблема быстрого перезапуска сервера
        int opt = 1;
        setsockopt(this->getSocketId(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // Привязываем сокет к адресу и порту (bind)
        int bindRes = bind(this->getSocketId(), (struct sockaddr*)&address, sizeof(address));
        if (bindRes < 0) {
            AppException::Throw("Привязка сокета (bind) завершилась ошибкой");
        }

        return bindRes;
    }

    int listenSocket(int requestSize = 10)
    {
        // Переводим сокет в режим прослушивания (listen)
        // 10 - это размер очереди "недообработанных" подключений (backlog)
        int listenRes = listen(this->getSocketId(), requestSize);
        if (listenRes < 0) {
            AppException::Throw("Перевод сокета в режим listen завершился ошибкой");
        }

        return listenRes;
    }
};

#endif //NBE_CHAT_BASEGSERVER_H
