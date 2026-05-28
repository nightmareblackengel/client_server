#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

using std::perror;
using std::cout;
using std::endl;

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

class Server01
{
private:
    int fileDescription;
public:
    Server01()
    {
        this->fileDescription = DEFAULT_INVALID_DESCRIPTOR;
    }
    ~Server01()
    {
        this->closeFd();
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

    int closeFd()
    {
        if (this->fileDescription == DEFAULT_INVALID_DESCRIPTOR) {
            return 0;
        }

        int closeRes = close(this->fileDescription);
        if (closeRes == 0) {
            this->fileDescription = DEFAULT_INVALID_DESCRIPTOR;
            cout << "Сервер остановлен." << endl;
        } else {
            cout << "CloseRes = " << closeRes << endl;
        }

        return closeRes;
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

        std::cout << "Сервер успешно запущен и слушает порт " << SERVER_PORT << "..." << endl;
        std::cout << "У вас есть 15 секунд, чтобы проверить его статус в терминале." << endl;

        // Временно усыпляем процесс на 15 секунд, чтобы он не закрылся сразу
        sleep(5);

        return 0;
    }
};

int main() {
    return Server01::runServer01();
}
