#ifndef NBE_CHAT_SERVER_BASETCPTRANSMITTER_H
#define NBE_CHAT_SERVER_BASETCPTRANSMITTER_H

#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "bootstrap.h"
#include "interfaces/ISocket.h"

using std::cout;
using std::endl;

const char* SERVER_HOST     = "127.0.0.1";
const int SERVER_PORT       = 8899;

class BaseTcpTransmitter: public ISocket
{
protected:
    string errorType = "None";
public:
    BaseTcpTransmitter():
            ISocket()
    {

    }

    ~BaseTcpTransmitter()
    {

    }

    int createSocket()
    {
        //  Создаем TCP-сокет
        // AF_INET - протокол IPv4
        // SOCK_STREAM - потоковый тип сокета (гарантирует доставку TCP)
        // 0           - автоматический выбор протокола (для SOCK_STREAM это всегда TCP)
        int socketId = socket(SERVER_IP_TYPE, SOCK_STREAM, 0);
        if (socketId == DEFAULT_INVALID_DESCRIPTOR) {
            this->throwException("Не удалось создать сокет");
        }
        this->setSocketId(socketId);

        return 0;
    }

    int connectToServer()
    {
        // Настройка структуры адреса сервера
        sockaddr_in address = this->getConfiguredAddress();

        if (connect(this->getSocketId(), (struct sockaddr*) &address, sizeof(address)) == -1) {
            this->throwException("Не удалось подключиться к серверу");
        }

        return 0;
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
            throw Cs01Exception("Привязка сокета (bind) завершилась ошибкой");
        }

        return bindRes;
    }

    int listenSocket(int requestSize = 10)
    {
        // Переводим сокет в режим прослушивания (listen)
        // 10 - это размер очереди "недообработанных" подключений (backlog)
        int listenRes = listen(this->getSocketId(), requestSize);
        if (listenRes < 0) {
            throw Cs01Exception("Перевод сокета в режим listen завершился ошибкой");
        }

        return listenRes;
    }

    sockaddr_in getConfiguredAddress()
    {
        // Настройка структуры адреса сервера
        sockaddr_in address{};
        address.sin_family  = SERVER_IP_TYPE;
        address.sin_port    = htons(SERVER_PORT);

        // Преобразуем строковый IP "127.0.0.1"(SERVER_HOST) в бинарный формат и записываем в структуру
        if (inet_pton(SERVER_IP_TYPE, SERVER_HOST, &address.sin_addr) <= 0) {
            this->throwException("Неверный IP-адрес или адрес не поддерживается");
        }

        return address;
    }

    void throwException(const string &msg)
    {
        throw Cs01Exception(msg, this->errorType);
    }
};

#endif //NBE_CHAT_SERVER_BASETCPTRANSMITTER_H
