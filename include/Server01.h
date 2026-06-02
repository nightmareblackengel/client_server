#ifndef CHAT_SERVER_SERVER01_H
#define CHAT_SERVER_SERVER01_H

#include <iostream>
#include <map>
#include <vector>
#include <unistd.h>
#include "bootstrap.h"
#include "ServerClient01.h"

using std::map;
using std::vector;
using std::perror;
using std::cout;
using std::endl;

class Server01
{
private:
    int socketId;
    map<int, ServerClient01*> clientList;
public:
    Server01()
    {
        this->socketId = DEFAULT_INVALID_DESCRIPTOR;
    }
    ~Server01()
    {
        this->closeFd();
        // & -> used for set nullptr to clientLIst item
        for (auto &item: this->clientList) {
            if (item.second != nullptr) {
                delete item.second;
                item.second = nullptr;
            }
        }
    }

    int createSocket()
    {
        //  Создаем TCP-сокет
        // AF_INET - протокол IPv4
        // SOCK_STREAM - потоковый тип сокета (гарантирует доставку TCP)
        this->socketId = socket(SERVER_IP_TYPE, SOCK_STREAM, 0);
        if (this->socketId == DEFAULT_INVALID_DESCRIPTOR) {
            perror("Не удалось создать сокет");
            return ERROR_CANT_CREATE_SOCKET;
        }
        cout << "удалось создать сокет" << endl;
        return this->socketId;
    }

    int bindSocket(sockaddr_in &address)
    {
        // Привязываем сокет к адресу и порту (bind)
        int bindRes = bind(this->socketId, (struct sockaddr*)&address, sizeof(address));
        if (bindRes < 0) {
            perror("Привязка сокета (bind) завершилась ошибкой");
            return ERROR_CANT_BIND_SOCKET;
        }

        return bindRes;
    }

    int listenSocket(int requestSize = 10)
    {
        // 4. Переводим сокет в режим прослушивания (listen)
        // 10 - это размер очереди "недообработанных" подключений (backlog)
        int listenRes = listen(this->socketId, requestSize);
        if (listenRes < 0) {
            perror("Перевод сокета в режим listen завершился ошибкой");
            return ERROR_CANT_START_LISTEN;
        }

        return listenRes;
    }

    int acceptNewClient()
    {
        ServerClient01 *c1 = new ServerClient01();

        cout << "Ожидание входящего подключения (accept)..." << endl;
        int clientId = c1->connectToServer(this->socketId);
        if (clientId >= 0) {
            this->clientList[clientId] = c1;
        } else {
            delete c1;
            c1 = nullptr;
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
            perror("Ошибка при чтении данных (recv)");
            return ERROR_WHEN_CLIENT_READ;
        }
        if (bytesRead == 0) {
            cout << "Клиент отключился до отправки данных." << endl;
            return ERROR_CLIENT_DISCONNECT;
        }
        ///
        std::string s1 (buff.data(), bytesRead);

        cout << "Получено от клиента [" << clientId << "](" << bytesRead << " байт):" << endl;
        cout << s1 << endl;
        cout << "----------------------------------------" << endl;

        return 0;
    }

    int closeFd()
    {
        if (this->socketId == DEFAULT_INVALID_DESCRIPTOR) {
            return 0;
        }

        int closeRes = close(this->socketId);
        if (closeRes == 0) {
            cout << "Сервер остановлен." << endl;
        } else {
            cout << "Сервер CloseRes = " << closeRes << endl;
        }
        this->socketId = DEFAULT_INVALID_DESCRIPTOR;
        cout << "Соединение с клиентом закрыто." << endl;

        return closeRes;
    }

    void removeClient(int clientId)
    {
        delete this->clientList[clientId];
        this->clientList.erase(clientId);
    }

    static int runServer01() {
        Server01 srv;
        if (srv.createSocket() < 0) {
            return ERROR_CANT_CREATE_SOCKET;
        };

        // 2. Настраиваем структуру адреса (куда привязать сервер)
        sockaddr_in address = getConfiguredAddress();
        if (srv.bindSocket(address) < 0) {
            return ERROR_CANT_BIND_SOCKET;
        }

        if (srv.listenSocket() < 0) {
            return ERROR_CANT_START_LISTEN;
        }

        // todo: this server used in Debian with systemd
        bool isServerRun = true;

        while (isServerRun) {

            int clientId = srv.acceptNewClient();
            if (clientId < 0) {
                return ERROR_CLIENT_CANT_ACCEPT;
            }

            int readRes = 0;
            while (readRes == 0) {
                cout << "readRes = " << readRes << endl;
                readRes = srv.readFromClient(clientId);
            }
            cout << "start removing client [" << clientId << "]" << endl;
            srv.removeClient(clientId);
            cout << "client [" << clientId << "] removed" << endl;
        }

        return 0;
    }
};

#endif //CHAT_SERVER_SERVER01_H
