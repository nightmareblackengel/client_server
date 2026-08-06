#ifndef CHAT_SERVER_SERVER01_H
#define CHAT_SERVER_SERVER01_H

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>
#include "bootstrap.h"
#include "ServerClient01.h"
#include "exceptions/Cs01Exception.h"
#include "BaseTcpTransmitter.h"
#include "composition/IServerClientList.h"
#include "composition/MyThreadPool.h"

using std::map;
using std::vector;
using std::perror;
using std::cout;
using std::endl;
using std::thread;
using std::atomic;
using std::signal;

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

        cout << "Ожидание входящего подключения (accept)..." << endl;
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

    int readFromClient(int clientId)
    {
        vector<char> buff(65536);

        // recv() читает данные из сокета клиента.
        // Он тоже блокирующий: ждет, пока клиент что-то пришлет.
        ssize_t bytesRead = recv(clientId, buff.data(), buff.size() - 1, 0);
        if (bytesRead < 0) {
            this->throwException("Ошибка при чтении данных (recv)");
        }
        if (bytesRead == 0) {
            this->throwException("Клиент отключился до отправки данных.");
        }

        std::string s1 (buff.data(), bytesRead);

        cout << "Получено от клиента [" << clientId << "](" << bytesRead << " байт):" << endl;
        cout << s1 << endl;
        cout << "----------------------------------------" << endl;

        return 0;
    }

    void acceptClientInThread(int clientId)
    {
        cout << "client [" << clientId << "] успешно подключился" << endl;
        auto clientConnectStartAt = std::chrono::steady_clock::now();
        int readRes = 0;
        while (readRes == 0) {
            try {
                readRes = this->readFromClient(clientId);
                cout << "client [" << clientId << "] прочитали следующее = " << readRes << endl;
            } catch (Cs01Exception& ex) {
                cout << "client [" << clientId << "] Заметка:" << ex.toString() << endl;
                readRes = -1;
            }
        }
        auto clientConnectEndAt = std::chrono::steady_clock::now();

        cout << "client [" << clientId << "] начало закрытие клиента [" << clientId << "]. Timeout = ["
             << std::chrono::duration_cast<std::chrono::milliseconds>(clientConnectEndAt - clientConnectStartAt) << "]" << endl;
        this->connectedClients.remove(clientId);
        cout << "client [" << clientId << "] закрыт" << endl;
    }

    static void runExitHandlers(int signum)
    {
        cout << "[Signal] Получен сигнал " << signum << ". Инициируем вежливую остановку..." << endl;
        Server01::inst->isRun = false;
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
                cout << "Ошибка:" << ex.toString() << endl;
            }
        }
        catch(const char* msg) {
            cout << "ОШИБКА. Подробная информация: " << msg << endl;
        }
        catch(...) {
            cout << "ОШИБКА. Экстренный выход из программы. Без дополнительной информации."<< endl;
        }

        this->Close();

        return 0;
    }
};

Server01* Server01::inst = nullptr;

#endif //CHAT_SERVER_SERVER01_H
