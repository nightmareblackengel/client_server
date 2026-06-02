#ifndef NBE_CHAT_SERVER_BASETCPTRANSMITTER_H
#define NBE_CHAT_SERVER_BASETCPTRANSMITTER_H

#include <netinet/in.h>
#include "bootstrap.h"

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
        if (this->socketId == DEFAULT_INVALID_DESCRIPTOR) {
            return;
        }

        this->closeSocket();
    }

    int closeSocket()
    {
        close(this->socketId);
        this->socketId = DEFAULT_INVALID_DESCRIPTOR;

        return 0;
    }

    int createSocket()
    {
        // AF_INET     - семейство адресов IPv4
        // SOCK_STREAM - тип сокета, обеспечивающий надежную потоковую передачу (TCP)
        // 0           - автоматический выбор протокола (для SOCK_STREAM это всегда TCP)
        this->socketId = socket(SERVER_IP_TYPE, SOCK_STREAM, 0);
        if (this->socketId == DEFAULT_INVALID_DESCRIPTOR) {
            // throw "Some EXception";
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
