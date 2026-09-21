#ifndef NBE_CHAT_SERVER_BASETCPTRANSMITTER_H
#define NBE_CHAT_SERVER_BASETCPTRANSMITTER_H

#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include "common/bootstrap.h"
#include "common/interfaces/RaiiBlockingSocket.h"

using std::cout;
using std::endl;
using std::vector;

const char* SERVER_HOST     = "127.0.0.1";
const int SERVER_PORT       = 8899;

class BaseTcpTransmitter
{
protected:
    string errorType = "None";
    RaiiBlockingSocket bSocket;
public:
    BaseTcpTransmitter()
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
        this->bSocket.setId(socketId);

        return 0;
    }

    int connectToSocket()
    {
        // Настройка структуры адреса сервера
        sockaddr_in address = this->getConfiguredAddress();

        if (connect(this->bSocket.getId(), (struct sockaddr*) &address, sizeof(address)) == -1) {
            this->throwException("Не удалось подключиться к серверу");
        }

        return 0;
    }

    int bindSocket()
    {
        sockaddr_in address = this->getConfiguredAddress();
        // Проблема быстрого перезапуска сервера
        int opt = 1;
        setsockopt(this->bSocket.getId(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // Привязываем сокет к адресу и порту (bind)
        int bindRes = bind(this->bSocket.getId(), (struct sockaddr*)&address, sizeof(address));
        if (bindRes < 0) {
            throw Cs01Exception("Привязка сокета (bind) завершилась ошибкой");
        }

        return bindRes;
    }

    int listenSocket(int requestSize = 10)
    {
        // Переводим сокет в режим прослушивания (listen)
        // 10 - это размер очереди "недообработанных" подключений (backlog)
        int listenRes = listen(this->bSocket.getId(), requestSize);
        if (listenRes < 0) {
            throw Cs01Exception("Перевод сокета в режим listen завершился ошибкой");
        }

        return listenRes;
    }

    int sendStringToSocket(int socketId, string& msg)
    {
        int res = send(socketId, msg.c_str(), msg.length(), 0);
        if (res < 0) {
            throw Cs01Exception("Отправка сообщения завершилась ошибкой");
        }
        return res;
    }

    string readFromSocket(int socketId, ssize_t& countRes)
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

    bool setFileDescriptorNonBlockFlag(int fileDescriptor)
    {
        // 1. Получаем текущие флаги дескриптора
        int flags = fcntl(fileDescriptor, F_GETFL, 0);
        if (flags == -1) {
            this->throwException("fcntl F_GETFL failed");
            return false;
        }
        // 2. Устанавливаем флаг O_NONBLOCK поверх существующих
        if (fcntl(fileDescriptor, F_SETFL, flags | O_NONBLOCK) == -1) {
            this->throwException("fcntl F_SETFL O_NONBLOCK failed");
            return false;
        }
        // TODO: remove
        cout << "set non block success" << endl;
        return true;
    }


    void throwException(const string &msg, std::source_location loc = std::source_location::current())
    {
        throw Cs01Exception(msg, this->errorType, __func__, loc);
    }
};

#endif //NBE_CHAT_SERVER_BASETCPTRANSMITTER_H
