#ifndef CHAT_SERVER_SERVER01_H
#define CHAT_SERVER_SERVER01_H

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>
#include "common/bootstrap.h"
#include "server/ServerClient01.h"
#include "common/exceptions/Cs01Exception.h"
#include "common/BaseTcpTransmitter.h"
#include "common/composition/IServerClientList.h"
#include "common/composition/MyThreadPool.h"

using std::map;
using std::vector;
using std::perror;
using std::cout;
using std::endl;
using std::thread;
using std::atomic;
using std::signal;
using std::to_string;
using std::chrono::steady_clock;

class Server01: public BaseTcpTransmitter
{
private:
    IServerClientList connectedClients;
    MyThreadPool tPool;
public:
    atomic<bool> isRun = true;
    static Server01* inst;

    Server01(int threadPoolCount=5): BaseTcpTransmitter(), tPool(threadPoolCount)
    {
        this->errorType = CS01_SERVER_TYPE;
    }

    ~Server01()
    {
        this->connectedClients.closeAll();
    }

    int acceptNewClient()
    {
        ServerClient01 *c1 = new ServerClient01();

        cout << "Ожидание подключения нового клиента (accept)..." << endl;
        int clientId = -1;
        try {
            clientId = c1->acceptFromServer(this->socketId);
            this->connectedClients.addClient(clientId, c1);
        } catch(Cs01Exception &ex1) {
            delete c1;
            c1 = nullptr;
            this->throwException("Не получилось присоединить клиента");
        }

        return clientId;
    }

    string readFromSocket(int clientId, ssize_t& countRes)
    {
        vector<char> buff(65536);

        // recv() читает данные из сокета клиента.
        // Он тоже блокирующий: ждет, пока клиент что-то пришлет.
        countRes = recv(clientId, buff.data(), buff.size() - 1, 0);
        if (countRes < 0) {
            this->throwException("Ошибка при чтении данных (recv)");
        }
        // countRes = 0 - нормальный выход
//        if (countRes == 0) {
//            this->throwException("клиент[" + to_string(clientId) + "] отключился до отправки данных.");
//        }

        string result(buff.data(), countRes);

        return result;
    }

    void acceptClientInThread(int clientId)
    {
        auto clientConnectStartAt = steady_clock::now();

        ssize_t readLenRes = 0;
        bool isClientSentMessages = true;
        std::stringstream threadStream;
        threadStream << "В потоке=[" <<  std::this_thread::get_id() << "] ";

        while (isClientSentMessages) {
            try {
                // read from client
                string readMessage = this->readFromSocket(clientId, readLenRes);
                // будем выходить если пустое сообщение
                if (readLenRes == 0) {
                    isClientSentMessages = false;
                    continue;
                }

                cout << threadStream.str() << "клиент [" << clientId << "] отправил сообщение:" << endl;
                cout << threadStream.str() << IoTextColor::CYAN << readMessage << IoTextColor::DEFAULT << endl;
                cout << threadStream.str() << "----------------------------------------" << endl;
                // отправим сообщение остальным пользователям
                // TODO: add as Task
                this->connectedClients.sendStringToOtherClients(clientId, readMessage);
            } catch (Cs01Exception& ex) {
                cout << IoTextColor::RED << "client [" << clientId << "] Заметка:" << ex.toString() << IoTextColor::DEFAULT << endl;
                isClientSentMessages = false;
            }
        }
        auto clientConnectEndAt = steady_clock::now();

        cout << threadStream.str() << IoTextColor::TEXT_ATTR_BOLD << IoTextColor::TEXT_ATTR_UNDERLINE
            << "Начало закрытие клиента [" << clientId << "]. Время = ["
            << std::chrono::duration_cast<std::chrono::seconds>(clientConnectEndAt - clientConnectStartAt) << "]"
            << IoTextColor::TEXT_ATTR_RESET << IoTextColor::DEFAULT << endl;
        this->connectedClients.remove(clientId);
    }

    static void runExitHandlers(int signum)
    {
        cout << "[Signal] Получен сигнал " << signum << ". Инициируем вежливую остановку..." << endl;
        Server01::inst->isRun = false;

        // остановим клиентов
        Server01::inst->connectedClients.terminateAnyDataTransmit();
        // остановим сервер
        shutdown(Server01::inst->socketId, SHUT_RDWR);
    }

    // Регистрируем обработчик для SIGINT (Ctrl+C / кнопка Stop в CLion)
    // и для SIGTERM (команда kill в Linux)
    void registerExitHandlers()
    {
        signal(SIGINT, Server01::runExitHandlers);
        signal(SIGTERM, Server01::runExitHandlers);
    }

    int run()
    {
        cout << IoTextColor::DEFAULT << "Сервер стартует..." << endl;
        Server01* app = Server01::inst;
        try {
            app->registerExitHandlers();

            app->createSocket();
            app->bindSocket();
            app->listenSocket();

            while (app->isRun) {
                // это должно выполнятся вне потока и блокироваться, чтобы не выполнять многократно  acceptNewClient - без наличия клиента
                int clientId = app->acceptNewClient();

                app->tPool.enqueueTask([&app, clientId] {
                    app->acceptClientInThread(clientId);
                });
            }
        }
        catch (Cs01Exception& ex) {
            // если ошибка во время оставновки - не показываем
            if (Server01::inst->isRun)
            {
                cout << IoTextColor::RED << "Ошибка:" << ex.toString() << IoTextColor::DEFAULT << endl;
            }
        }
        catch(const char* msg) {
            cout << IoTextColor::RED << "ОШИБКА. Подробная информация: " << msg << IoTextColor::DEFAULT<< endl;
        }
        catch(...) {
            cout << IoTextColor::RED << "ОШИБКА. Экстренный выход из программы. Без дополнительной информации." << IoTextColor::DEFAULT << endl;
        }

        return 0;
    }
};

Server01* Server01::inst = nullptr;

#endif //CHAT_SERVER_SERVER01_H
