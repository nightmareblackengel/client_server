#ifndef NBE_CHAT_SERVER_BASETCPTRANSMITTER_H
#define NBE_CHAT_SERVER_BASETCPTRANSMITTER_H

#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include "common/bootstrap.h"
#include "common/exceptions/Cs01Exception.h"
#include "common/raii/RaiiSocket.h"

using std::cout;
using std::endl;
using std::vector;

const char* SERVER_HOST     = "127.0.0.1";
const int SERVER_PORT       = 8899;

class AbstractTransmitter
{
protected:
    string errorType = "None";
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

    void throwException(const string &msg, std::source_location loc = std::source_location::current())
    {
        throw Cs01Exception(msg, this->errorType, __func__, loc);
    }
};

#endif //NBE_CHAT_SERVER_BASETCPTRANSMITTER_H
