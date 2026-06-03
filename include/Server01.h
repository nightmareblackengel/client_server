#ifndef CHAT_SERVER_SERVER01_H
#define CHAT_SERVER_SERVER01_H

#include <iostream>
#include <map>
#include <vector>
#include <chrono>
#include "bootstrap.h"
#include "ServerClient01.h"
#include "Cs01Exception.h"
#include "BaseTcpTransmitter.h"

using std::map;
using std::vector;
using std::perror;
using std::cout;
using std::endl;

class Server01: public BaseTcpTransmitter
{
private:
    map<int, ServerClient01*> clientList;
public:
    Server01(): BaseTcpTransmitter()
    {
        this->errorType = CS01_SERVER_TYPE;
    }
    ~Server01()
    {
        // & -> used for set nullptr to clientLIst item
        for (auto &item: this->clientList) {
            if (item.second != nullptr) {
                delete item.second;
                item.second = nullptr;
            }
        }
    }

    int acceptNewClient()
    {
        ServerClient01 *c1 = new ServerClient01();

        cout << "Ожидание входящего подключения (accept)..." << endl;
        int clientId = -1;
        try {
            clientId = c1->acceptFromServer(this->socketId);
            this->clientList[clientId] = c1;
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

    void removeClient(int clientId)
    {
        delete this->clientList[clientId];
        this->clientList.erase(clientId);
    }

    static int runServer01() {
        try {
            Server01 srv;
            srv.createSocket();
            srv.bindSocket();

            srv.listenSocket();

            // todo: this server used in Debian with systemd
            bool isServerRun = true;

            while (isServerRun) {
                int clientId = srv.acceptNewClient();
                auto clientConnectStartAt = std::chrono::steady_clock::now();
                // TODO:
                int readRes = 0;
                while (readRes == 0) {
                    cout << "readRes = " << readRes << endl;
                    try {
                        readRes = srv.readFromClient(clientId);
                    } catch (Cs01Exception& ex) {
                        cout << "Ошибка:" << ex.toString() << endl;
                        readRes = -1;
                    }
                }
                auto clientConnectEndAt = std::chrono::steady_clock::now();

                cout << "start removing client [" << clientId << "]. Timeout = ["
                    << std::chrono::duration_cast<std::chrono::milliseconds>(clientConnectEndAt - clientConnectStartAt) << "]" << endl;
                srv.removeClient(clientId);
                cout << "client [" << clientId << "] removed" << endl;
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
