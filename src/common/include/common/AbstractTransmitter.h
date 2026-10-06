#ifndef NBE_CHAT_SERVER_BASETCPTRANSMITTER_H
#define NBE_CHAT_SERVER_BASETCPTRANSMITTER_H

#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include "common/bootstrap.h"
#include "common/raii/RaiiSocket.h"
#include "common/static/AppException.h"

using std::cout;
using std::endl;
using std::vector;

const char* SERVER_HOST     = "127.0.0.1";
const int SERVER_PORT       = 8899;

class AbstractTransmitter
{
protected:
    RaiiSocket bSocket;
public:
    AbstractTransmitter()
    {

    }

    virtual ~AbstractTransmitter()
    {

    }

    virtual int createSocket() = 0;

    virtual int sendStringToSocket(int socketId, string& msg) = 0;
    virtual string readFromSocket(int socketId, ssize_t& countRes) = 0;

    int getSocketId()
    {
        return this->bSocket.getId();
    }
    void setSocketId(int _id)
    {
        this->bSocket.setId(_id);
    }

    sockaddr_in getConfiguredAddress()
    {
        // Настройка структуры адреса сервера
        sockaddr_in address{};
        address.sin_family  = SERVER_IP_TYPE;
        address.sin_port    = htons(SERVER_PORT);

        // Преобразуем строковый IP "127.0.0.1"(SERVER_HOST) в бинарный формат и записываем в структуру
        if (inet_pton(SERVER_IP_TYPE, SERVER_HOST, &address.sin_addr) <= 0) {
            AppException::Throw("Неверный IP-адрес или адрес не поддерживается");
        }

        return address;
    }
};

#endif //NBE_CHAT_SERVER_BASETCPTRANSMITTER_H
