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

using std::map;
using std::vector;
using std::perror;
using std::cout;
using std::endl;
using std::thread;
using std::atomic;
using std::signal;

atomic<bool> isServerRun;
void runExitHandlers(int signum)
{
    cout << "[Signal] Получен сигнал " << signum << ". Инициируем вежливую остановку..." << endl;
    isServerRun = false;
}

class Server01: public BaseTcpTransmitter
{
private:
    IServerClientList connectedClients;
    vector<thread> threadPool;
public:

    Server01(): BaseTcpTransmitter()
    {
        this->errorType = CS01_SERVER_TYPE;
        isServerRun = true;
    }
    ~Server01()
    {
        cout << "Server01 Desctructor" << endl;
        for (thread &t1 : this->threadPool) {
            if (t1.joinable()) {
                t1.join();
            }
        }
        this->threadPool.clear();
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
        cout << "client [" << clientId << "] accepted success" << endl;
        auto clientConnectStartAt = std::chrono::steady_clock::now();
        int readRes = 0;
        while (readRes == 0) {
            try {
                readRes = this->readFromClient(clientId);
                cout << "client [" << clientId << "] ReadRes = " << readRes << endl;
            } catch (Cs01Exception& ex) {
                cout << "client [" << clientId << "] Заметка:" << ex.toString() << endl;
                readRes = -1;
            }
        }
        auto clientConnectEndAt = std::chrono::steady_clock::now();

        cout << "client [" << clientId << "] start removing client [" << clientId << "]. Timeout = ["
             << std::chrono::duration_cast<std::chrono::milliseconds>(clientConnectEndAt - clientConnectStartAt) << "]" << endl;
        this->connectedClients.remove(clientId);
        cout << "client [" << clientId << "] removed" << endl;
    }

    // Регистрируем обработчик для SIGINT (Ctrl+C / кнопка Stop в CLion)
    // и для SIGTERM (команда kill в Linux)
    void registerExitHandlers()
    {
        signal(SIGINT, runExitHandlers);
        signal(SIGTERM, runExitHandlers);
    }

    static int runServer01() {
        try {
            Server01 srv;
            // TODO: move
            srv.registerExitHandlers();

            srv.createSocket();
            srv.bindSocket();
            srv.listenSocket();

            while (isServerRun) {
                // это должно выполнятся вне потока и блокироваться, чтобы не выполнять многократно  acceptNewClient - без наличия клиента
                int clientId = srv.acceptNewClient();

                srv.threadPool.push_back(thread(&Server01::acceptClientInThread, std::ref(srv), clientId));
            }
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

        return 0;
    }
};

#endif //CHAT_SERVER_SERVER01_H
