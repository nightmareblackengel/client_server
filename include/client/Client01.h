#ifndef NBE_CHAT_SERVER_CLIENT_H
#define NBE_CHAT_SERVER_CLIENT_H

#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include "common/bootstrap.h"
#include "exceptions/Cs01Exception.h"
#include "common/BaseTcpTransmitter.h"
#include <csignal>

using std::string;
using std::cout;
using std::endl;
using std::thread;
using std::atomic;

class Client01: public BaseTcpTransmitter
{
    atomic<bool> isRun = true;
    thread *sendMessageThr = nullptr;
    thread *getMessagesThr = nullptr;
public:
    static Client01 *inst;

    Client01(): BaseTcpTransmitter()
    {
        this->errorType = CS01_CLIENT_TYPE;
    }

    ~Client01()
    {
//        cout << "Destructor sendMessageThr... " << "" << endl;
        if (this->sendMessageThr != nullptr) {
//            cout << "check joinable..." << endl;
            if (this->sendMessageThr->joinable()) {
//                cout << "join started..." << endl;
                // TODO: create situation where this throw exception
                this->sendMessageThr->join();
            }
//            cout << "delete started..." << endl;
            delete this->sendMessageThr;
        }
        if (this->getMessagesThr != nullptr) {
            delete this->getMessagesThr;
        }
    }

    bool getIsRun() const
    {
        return this->isRun;
    }
    void setIsRun(bool v)
    {
        this->isRun = v;
    }

    void sendMessageToServerHandler()
    {
        Client01 *app = this->inst;

        while (app->getIsRun() == true) {
            cout << "Введите сообщение для отправки:" << endl;
            string newLine;
            std::getline(std::cin, newLine);
//            cout << "after getline position" << endl;
            // если сообщение пустое - будет считаться как выход
            if (newLine.empty()) {
                app->setIsRun(false);
                continue;
            }
            int sendRes = app->sendStringToSocket(this->socketId, newLine);
        }

        return;
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

            this->sendMessageThr = new thread(&Client01::sendMessageToServerHandler, this);
            // TODO: thread 2 start// t2 = listen

            if (this->sendMessageThr->joinable()) {
                this->sendMessageThr->join();
            }
            //cout << "JOIN ended" << endl;
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
        //cout << "RUN ENDED ...."<< endl;
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

        // в той ситуации когда пользователь захотел закрыть программу можно
        // Закрыть stdin
        // getline() -> получит EOF.
        // Но это грубый способ — после него консоль уже не работает.
//        cout << "console stop started..." << endl;
        // not work
        close(STDIN_FILENO);
//        cout << "console stop ended..." << endl;

        // TODO: remove if not use
        // "Больше не передавай и не принимай данные."
        // Но сам файловый дескриптор остается существовать.
        // shutdown(item.first, SHUT_RDWR);
//        cout << "cant stop the code" << endl;
    }
};

Client01* Client01::inst = nullptr;
// TODO:
// add throw in Client01::sendMessageToServerHandler or add try catch

#endif //NBE_CHAT_SERVER_CLIENT_H
// TODO:
// Часть 1. Модернизация Клиента (Двухпоточный клиент)
//Перепиши свой консольный клиент так, чтобы после успешного connect() он порождал один вспомогательный поток (std::thread):
//
//Главный поток (UI / Отправка): Занимается только тем, что в бесконечном цикле ждет ввода пользователя через std::getline и сразу отправляет строку в сокет через send().
//
//Вспомогательный поток (Слушатель / Прием): Занимается только тем, что в бесконечном цикле вызывает блокирующий recv() на сокете клиента. Как только данные пришли — он тут же выводит их в std::cout.
//
//Подсказка: так как сокет в Linux дескрипторно потокобезопасен на разделение чтения/записи, один поток может спокойно делать send, пока другой висит в recv на том же самом сокете.
