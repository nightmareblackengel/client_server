#ifndef NBE_CHAT_SERVER_CLIENT_H
#define NBE_CHAT_SERVER_CLIENT_H

#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <sstream>
#include "common/bootstrap.h"
#include "common/exceptions/Cs01Exception.h"
#include "common/transmitter/BaseClient.h"
#include "common/interfaces/LinuxExitHandlers.h"

using std::string;
using std::cout;
using std::endl;
using std::thread;
using std::atomic;

class Client01: public BaseClient, public LinuxExitHandlers<Client01>
{
    atomic<bool> isRun = true;
    thread *sendMessageThr = nullptr;
    thread *getMessagesThr = nullptr;
public:
    static Client01 *inst;

    Client01(): BaseClient()
    {

    }

    ~Client01()
    {
//        cout << "Destructor sendMessageThr... " << "" << endl;
        this->freeSendMsgThread();
        this->freeGetMsgThread();
    }

    void freeGetMsgThread()
    {
        if (this->getMessagesThr != nullptr) {
            if (this->getMessagesThr->joinable()) {
                this->getMessagesThr->join();
            }
            delete this->getMessagesThr;
            this->getMessagesThr = nullptr;
        }
    }
    void freeSendMsgThread()
    {
        if (this->sendMessageThr != nullptr) {
//            cout << "check joinable..." << endl;
            if (this->sendMessageThr->joinable()) {
//                cout << "join started..." << endl;
                // TODO: create situation where this throw exception
                this->sendMessageThr->join();
            }
//            cout << "delete started..." << endl;
            delete this->sendMessageThr;
            this->sendMessageThr = nullptr;
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
            int sendRes = app->sendStringToSocket(this->bSocket.getId(), newLine);
        }

        return;
    }

    void getMessageFromHandler()
    {
        // TODO: send message to closed client
        Client01 *app = this->inst;

        std::stringstream threadStream;
        threadStream << "В потоке=[" <<  std::this_thread::get_id() << "] ";
        ssize_t readLenRes = 0;
        int socketId = app->bSocket.getId();
        while (app->getIsRun() == true) {
            string readMessage = Client01::readFromSocket(socketId, readLenRes);
            if (!readMessage.empty()) {
                cout << threadStream.str() << "клиент отправил сообщение:" << endl;
                cout << threadStream.str() << IoTextColor::CYAN << readMessage << IoTextColor::DEFAULT << endl;
                cout << threadStream.str() << "----------------------------------------" << endl;
            } else {
                cout << IoTextColor::RED << "ОШИБКА. Сообщение содержит пустую строку " << IoTextColor::DEFAULT << endl;
            }
        }
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
            app->connectToSocket();
            cout << "Успешно подключено к серверу" << endl;

            this->sendMessageThr = new thread(&Client01::sendMessageToServerHandler, this);
            this->getMessagesThr = new thread(&Client01::getMessageFromHandler, this);

            this->freeSendMsgThread();
            this->freeGetMsgThread();
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

#endif //NBE_CHAT_SERVER_CLIENT_H
