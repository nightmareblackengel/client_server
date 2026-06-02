#ifndef NBE_CHAT_SERVER_CLIENT_H
#define NBE_CHAT_SERVER_CLIENT_H

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "bootstrap.h"
#include "Cs01Exception.h"

using std::string;
using std::cout;
using std::endl;

class Client01
{
private:
    int socketId;
public:
    Client01()
    {
        this->socketId = DEFAULT_INVALID_DESCRIPTOR;
    }

    ~Client01()
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
            throw Cs01ClientException("Не удалось создать сокет");
        }

        return 0;
    }

    int connectToServer()
    {
        // Настройка структуры адреса сервера
        sockaddr_in serverAddress{};
        serverAddress.sin_family = SERVER_IP_TYPE;
        serverAddress.sin_port = htons(SERVER_PORT);

        // Преобразуем строковый IP "127.0.0.1"(SERVER_HOST) в бинарный формат и записываем в структуру
        if (inet_pton(SERVER_IP_TYPE, SERVER_HOST, &serverAddress.sin_addr) <= 0) {
            throw Cs01ClientException("Неверный IP-адрес или адрес не поддерживается");
        }

        if (connect(this->socketId, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) == -1) {
            throw Cs01ClientException("Не удалось подключиться к серверу");
        }

        return 0;
    }

    int getSocketId()
    {
        return this->socketId;
    }

    static void run()
    {
        cout << "Запуск консольного клиента..." << endl;
        //
        try {
            Client01 c01;
            // Создание сокета
            c01.createSocket();

            cout << "Попытка подключения к серверу (127.0.0.1:8899)..." << endl;
            // Подключение к серверу
            c01.connectToServer();

            cout << "Успешно подключено к серверу" << endl;
            cout << "Удерживаем соединение 3 секунды..." << endl;
            sleep(3);
            c01.closeSocket();

            cout << "Сокет закрыт. Завершение работы." << endl;
        }
        catch (Cs01Exception& ex) {
            cout << "Ошибка:" << ex.toString() << endl;
        }
        catch(const char* msg) {
            cout << "ОШИБКА. Подробная информация: " << msg << endl;
        }
        catch(...) {
            cout << "ОШИБКА. Экстренный выход из программы. Без дополнительной информации."<< endl;
        }
    }
};

#endif //NBE_CHAT_SERVER_CLIENT_H
