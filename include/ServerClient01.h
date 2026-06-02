#ifndef CHAT_SERVER_SERVERCLIENT01_H
#define CHAT_SERVER_SERVERCLIENT01_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "bootstrap.h"

using std::cout;
using std::endl;
using std::perror;


class ServerClient01
{
private:
    int socketId;
    sockaddr_in addr{};
    socklen_t   addrLen{};
public:
    ServerClient01()
    {
        this->socketId = DEFAULT_INVALID_DESCRIPTOR;
        this->addrLen = sizeof(this->addr);
    }
    // 1. ЗАПРЕЩАЕМ копирование (чтобы случайно не скопировать сокет)
    ServerClient01(const ServerClient01&) = delete;
    ServerClient01& operator=(const ServerClient01&) = delete;
    // 2. РАЗРЕШАЕМ перемещение (Конструктор перемещения)
    ServerClient01(ServerClient01&& other) = delete;
    // 3. Оператор присваивания перемещением
    ServerClient01& operator=(ServerClient01&& other) = delete;

    ~ServerClient01()
    {
        this->closeFd();
    }

    int connectToServer(int serverFd)
    {
        // accept блокирует поток, пока кто-то не подключится
        this->socketId = accept(serverFd, (struct sockaddr*)&(this->addr), &(this->addrLen));
        if (this->socketId < 0) {
            perror("Client. Не удалось принять подключение (accept)");
            return -22;
        }
        cout << "Клиент успешно подключился! Дескриптор клиента: " << this->socketId << endl;
        return this->socketId;
    }

    int closeFd()
    {
        // Закрываем сокет клиента после общения
        if (this->socketId == DEFAULT_INVALID_DESCRIPTOR) {
            return 0;
        }
        int closeRes = close(this->socketId);
        if (closeRes != 0) {
            cout << "Client Error CloseRes = " << closeRes << endl;
        }
        this->socketId = DEFAULT_INVALID_DESCRIPTOR;

        return closeRes;
    }
};

#endif //CHAT_SERVER_SERVERCLIENT01_H
