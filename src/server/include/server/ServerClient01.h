#ifndef CHAT_SERVER_SERVERCLIENT01_H
#define CHAT_SERVER_SERVERCLIENT01_H

#include <iostream>
#include <netinet/in.h>
#include "common/bootstrap.h"
#include "common/exceptions/Cs01Exception.h"
#include "common/raii/RaiiSocket.h"

using std::cout;
using std::endl;
using std::perror;

class ServerClient01
{
private:
    sockaddr_in addr{};
    socklen_t   addrLen{};
    RaiiSocket socket;
public:
    ServerClient01()
    {
        this->addrLen = sizeof(this->addr);
    }
    // 1. ЗАПРЕЩАЕМ копирование (чтобы случайно не скопировать сокет)
    ServerClient01(const ServerClient01&) = delete;
    ServerClient01& operator=(const ServerClient01&) = delete;
    // 2. Конструктор перемещения
    ServerClient01(ServerClient01&& other) = delete;
    // 3. Оператор присваивания перемещением
    ServerClient01& operator=(ServerClient01&& other) = delete;

    ~ServerClient01()
    {

    }

    int acceptFromServer(int serverSocket)
    {
        // accept блокирует поток, пока кто-то не подключится
        int socketId = accept(serverSocket, (struct sockaddr*)&(this->addr), &(this->addrLen));
        if (socketId < 0) {
            throw Cs01Exception("Не удалось принять подключение (accept)");
        }
        this->socket.setId(socketId);
        cout << "Клиент успешно подключился! Дескриптор клиента: " << socketId << endl;
        return socketId;
    }
};

#endif //CHAT_SERVER_SERVERCLIENT01_H
