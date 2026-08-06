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
#include <csignal>

using std::string;
using std::cout;
using std::endl;

class Client01: public BaseTcpTransmitter
{
    bool isRun = true;
public:
    static Client01 *inst;

    Client01(): BaseTcpTransmitter()
    {
        this->errorType = CS01_CLIENT_TYPE;
    }

    ~Client01()
    {
    }

    bool getIsRun() const
    {
        return this->isRun;
    }
    void setIsRun(bool v)
    {
        this->isRun = v;
    }

    void run()
    {
        cout << IoTextColor::DEFAULT << "Запуск консольного клиента..." << endl;
        Client01 *app = this->inst;
        //
        try {
            this->registerExitHandlers();

            // Создание сокета
            app->createSocket();
            cout << "Сокет открыт." << endl;

            cout << "Попытка подключения к серверу (127.0.0.1:8899)..." << endl;
            // Подключение к серверу
            app->connectToServer();
            cout << "Успешно подключено к серверу" << endl;

            while (app->getIsRun() == true) {
                // TODO: add thread there
                // TODO: t1 = getline + sending
                {
                    cout << "Введите сообщение для отправки:" << endl;
                    string newLine;
                    std::getline(std::cin, newLine);
                    // если сообщение пустое - будет считаться как выход
                    if (newLine.length() == 0) {
                        app->setIsRun(false);
                        continue;
                    }
                    int sendRes = app->sendStringToSocket(newLine);
                }

                // TODO: t2 = listen
            }
        }
        catch (Cs01Exception& ex) {
            cout << IoTextColor::RED <<  "Ошибка:" << ex.toString() << IoTextColor::DEFAULT << endl;
        }
        catch(const char* msg) {
            cout << IoTextColor::RED << "ОШИБКА. Подробная информация: " << msg << IoTextColor::DEFAULT << endl;
        }
        catch(...) {
            cout << IoTextColor::RED << "ОШИБКА. Экстренный выход из программы. Без дополнительной информации." << IoTextColor::DEFAULT<< endl;
        }
    }

    void registerExitHandlers()
    {
        signal(SIGINT, Client01::runExitHandlers);
        signal(SIGTERM, Client01::runExitHandlers);
    }

    static void runExitHandlers(int signum)
    {
        if (Client01::inst == nullptr) {
            return;
        }
        cout << "[Signal] Получен сигнал " << signum << ". Инициируем вежливую остановку..." << endl;
        Client01::inst->setIsRun(false);
    }
};

Client01* Client01::inst = nullptr;

#endif //NBE_CHAT_SERVER_CLIENT_H

// Часть 1. Модернизация Клиента (Двухпоточный клиент)
//Перепиши свой консольный клиент так, чтобы после успешного connect() он порождал один вспомогательный поток (std::thread):
//
//Главный поток (UI / Отправка): Занимается только тем, что в бесконечном цикле ждет ввода пользователя через std::getline и сразу отправляет строку в сокет через send().
//
//Вспомогательный поток (Слушатель / Прием): Занимается только тем, что в бесконечном цикле вызывает блокирующий recv() на сокете клиента. Как только данные пришли — он тут же выводит их в std::cout.
//
//Подсказка: так как сокет в Linux дескрипторно потокобезопасен на разделение чтения/записи, один поток может спокойно делать send, пока другой висит в recv на том же самом сокете.
