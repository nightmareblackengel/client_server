#ifndef NBE_CHAT_SERVER_BASETCPTRANSMITTER_H
#define NBE_CHAT_SERVER_BASETCPTRANSMITTER_H

#include <iostream>
#include <netinet/in.h>
#include "bootstrap.h"

using std::cout;
using std::endl;

class BaseTcpTransmitter
{
public:
    const char* SERVER_HOST     = "127.0.0.1";
    const int SERVER_PORT       = 8899;

protected:
    int socketId;
public:
    BaseTcpTransmitter()
    {
        this->socketId = DEFAULT_INVALID_DESCRIPTOR;
    }

    ~BaseTcpTransmitter()
    {
        this->closeSocket();
    }

    int closeSocket()
    {
        if (this->socketId == DEFAULT_INVALID_DESCRIPTOR) {
            return 0;
        }

        int closeRes = close(this->socketId);
        if (closeRes == 0) {
            cout << "Сокет закрыт" << endl;
        } else {
            cout << "Сокет незакрыт. Код ошибки: " << closeRes << endl;
        }
        this->socketId = DEFAULT_INVALID_DESCRIPTOR;

        return closeRes;
    }

    int createSocket()
    {
        //  Создаем TCP-сокет
        // AF_INET - протокол IPv4
        // SOCK_STREAM - потоковый тип сокета (гарантирует доставку TCP)
        // 0           - автоматический выбор протокола (для SOCK_STREAM это всегда TCP)
        this->socketId = socket(SERVER_IP_TYPE, SOCK_STREAM, 0);
        if (this->socketId == DEFAULT_INVALID_DESCRIPTOR) {
            throw Cs01Exception("Не удалось создать сокет");
        }

        return 0;
    }

    int connectToServer()
    {
        // Настройка структуры адреса сервера
        sockaddr_in address = this->getConfiguredAddress();

        if (connect(this->socketId, (struct sockaddr*) &address, sizeof(address)) == -1) {
            throw Cs01Exception("Не удалось подключиться к серверу");
        }

        return 0;
    }

    int bindSocket()
    {
        sockaddr_in address = this->getConfiguredAddress();
        // Привязываем сокет к адресу и порту (bind)
        int bindRes = bind(this->socketId, (struct sockaddr*)&address, sizeof(address));
        if (bindRes < 0) {
            throw Cs01Exception("Привязка сокета (bind) завершилась ошибкой");
        }

        return bindRes;
    }

    int listenSocket(int requestSize = 10)
    {
        // Переводим сокет в режим прослушивания (listen)
        // 10 - это размер очереди "недообработанных" подключений (backlog)
        int listenRes = listen(this->socketId, requestSize);
        if (listenRes < 0) {
            throw Cs01Exception("Перевод сокета в режим listen завершился ошибкой");
        }

        return listenRes;
    }

    int getSocketId()
    {
        return this->socketId;
    }

    sockaddr_in getConfiguredAddress()
    {
        // Настройка структуры адреса сервера
        sockaddr_in address{};
        address.sin_family  = SERVER_IP_TYPE;
        address.sin_port    = htons(SERVER_PORT);

        // Преобразуем строковый IP "127.0.0.1"(SERVER_HOST) в бинарный формат и записываем в структуру
        if (inet_pton(SERVER_IP_TYPE, SERVER_HOST, &address.sin_addr) <= 0) {
            throw "Неверный IP-адрес или адрес не поддерживается";
        }

        return address;
    }
};

#endif //NBE_CHAT_SERVER_BASETCPTRANSMITTER_H
