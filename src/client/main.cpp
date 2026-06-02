#include <iostream>
#include <unistd.h>
#include <string>
#include "bootstrap.h"
#include "Cs01Exception.h"

using std::string;
using std::cout;
using std::endl;

class Client01
{
private:
    int socketId = -1;
public:
    Client01()
    {

    }

    ~Client01()
    {
        if (this->socketId == DEFAULT_INVALID_DESCRIPTOR) {
            return;
        }

        close(this->socketId);
        this->socketId = DEFAULT_INVALID_DESCRIPTOR;
    }

    int createSocket()
    {
        // AF_INET     - семейство адресов IPv4
        // SOCK_STREAM - тип сокета, обеспечивающий надежную потоковую передачу (TCP)
        // 0           - автоматический выбор протокола (для SOCK_STREAM это всегда TCP)
        this->socketId = socket(SERVER_IP_TYPE, SOCK_STREAM, 0);
        if (this->socketId == DEFAULT_INVALID_DESCRIPTOR) {
            // throw "Some EXception";
            throw Cs01ClientException("Не удалось создать сокет!");
        }

        return this->socketId;
    }

    int getSocketId()
    {
        return this->socketId;
    }

    static void run()
    {
        cout << "[КЛИЕНТ] Запуск консольного клиента..." << endl;
        //
        try {
            Client01 c01;
            // Шаг 1: Создание сокета
            c01.createSocket();

            cout << c01.getSocketId() << endl;
            cout << "[КЛИЕНТ] Сокет закрыт. Завершение работы." << endl;
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
    }
};

int main()
{
    Client01::run();


    return 0;
}
