#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <map>
#include <vector>

using std::perror;
using std::cout;
using std::endl;
using std::map;
using std::vector;

const int SERVER_PORT       = 8899;
const int SERVER_IP_TYPE    = AF_INET;

sockaddr_in getConfiguredAddress() {
    sockaddr_in address{};
    address.sin_family = SERVER_IP_TYPE;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);

    return address;
}

const int DEFAULT_INVALID_DESCRIPTOR    = -1;
const int ERROR_CANT_CREATE_SOCKET      = -2;
const int ERROR_CANT_BIND_SOCKET        = -3;
const int ERROR_CANT_START_LISTEN       = -4;
const int ERROR_CLIENT_CANT_ACCEPT      = -5;
const int ERROR_WHEN_CLIENT_READ        = -6;
const int ERROR_CLIENT_DISCONNECT       = -7;

class Client01
{
private:
    int fileDescription;
    sockaddr_in addr{};
    socklen_t   addrLen{};
public:
    Client01()
    {
        this->fileDescription = DEFAULT_INVALID_DESCRIPTOR;
        this->addrLen = sizeof(this->addr);
    }
    // 1. ЗАПРЕЩАЕМ копирование (чтобы случайно не скопировать сокет)
    Client01(const Client01&) = delete;
    Client01& operator=(const Client01&) = delete;
    // 2. РАЗРЕШАЕМ перемещение (Конструктор перемещения)
    Client01(Client01&& other) = delete;
    // 3. Оператор присваивания перемещением
    Client01& operator=(Client01&& other) = delete;

    ~Client01()
    {
        this->closeFd();
    }

    int connectToServer(int serverFd)
    {
        // accept блокирует поток, пока кто-то не подключится
        this->fileDescription = accept(serverFd, (struct sockaddr*)&(this->addr), &(this->addrLen));
        if (this->fileDescription < 0) {
            perror("Client. Не удалось принять подключение (accept)");
            return ERROR_CLIENT_CANT_ACCEPT;
        }
        cout << "Клиент успешно подключился! Дескриптор клиента: " << this->fileDescription << endl;
        return this->fileDescription;
    }

    int closeFd()
    {
        // Закрываем сокет клиента после общения
        if (this->fileDescription == DEFAULT_INVALID_DESCRIPTOR) {
            return 0;
        }
        int closeRes = close(this->fileDescription);
        if (closeRes != 0) {
            cout << "Client Error CloseRes = " << closeRes << endl;
        }
        this->fileDescription = DEFAULT_INVALID_DESCRIPTOR;

        return closeRes;
    }
};

class Server01
{
private:
    int fileDescription;
    map<int, Client01*> clientList;
public:
    Server01()
    {
        this->fileDescription = DEFAULT_INVALID_DESCRIPTOR;
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
        this->fileDescription = socket(SERVER_IP_TYPE, SOCK_STREAM, 0);
        if (this->fileDescription == -1) {
            perror("Не удалось создать сокет");
            return ERROR_CANT_CREATE_SOCKET;
        }
        cout << "удалось создать сокет" << endl;
        return this->fileDescription;
    }

    int bindSocket(sockaddr_in &address)
    {
        // Привязываем сокет к адресу и порту (bind)
        int bindRes = bind(this->fileDescription, (struct sockaddr*)&address, sizeof(address));
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
        int listenRes = listen(this->fileDescription, requestSize);
        if (listenRes < 0) {
            perror("Перевод сокета в режим listen завершился ошибкой");
            return ERROR_CANT_START_LISTEN;
        }

        return listenRes;
    }

    int acceptNewClient()
    {
        Client01 *c1 = new Client01();

        cout << "Ожидание входящего подключения (accept)..." << endl;
        int clientId = c1->connectToServer(this->fileDescription);
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
        if (this->fileDescription == DEFAULT_INVALID_DESCRIPTOR) {
            return 0;
        }

        int closeRes = close(this->fileDescription);
        if (closeRes == 0) {
            cout << "Сервер остановлен." << endl;
        } else {
            cout << "Сервер CloseRes = " << closeRes << endl;
        }
        this->fileDescription = DEFAULT_INVALID_DESCRIPTOR;
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

int main() {
    return Server01::runServer01();
}
