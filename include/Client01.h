#ifndef NBE_CHAT_SERVER_CLIENT_H
#define NBE_CHAT_SERVER_CLIENT_H

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include "bootstrap.h"
#include "Cs01Exception.h"
#include "BaseTcpTransmitter.h"

using std::string;
using std::cout;
using std::endl;

class Client01: public BaseTcpTransmitter
{

public:
    Client01(): BaseTcpTransmitter()
    {

    }

    ~Client01()
    {
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
            c01.Close();

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
